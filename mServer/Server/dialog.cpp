#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //
    server = new MyServer(this);

    connect(this, SIGNAL(sigMessageFromServer(QString&, const QStringList&)), server, SLOT(slotMessageFromServer(QString&, const QStringList&)));
    connect(server, SIGNAL(sigRemoveUserFromList(QString)), this, SLOT(slotRemoveUser(QString)));
    connect(server, SIGNAL(sigAddUserToList(QString)), this, SLOT(slotAddUser(QString)));
    connect(server, SIGNAL(sigMessageToLog(QString&,QString,const QStringList&)), this, SLOT(slotNewMessage(QString&,QString,const QStringList&)));
    connect(this, SIGNAL(sigServerStop()), server, SLOT(slotServerStop()));

    QHostAddress addr;
    addr.setAddress(ui->leHost->text());

    if (server->startServer(addr, ui->lePort->text().toInt()))
    {
        addToLog("Server started at " + server->serverAddress().toString() + "/" + QString::number(server->serverPort()), Qt::darkGreen);
    }
    else
    {
        addToLog("Server can't start at 127.0.0.1:" + QString::number(server->serverPort()), Qt::darkRed);
        ui->pbStartStop->setText("Start server");
    }
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::slotAddUser(QString name)
{
    ui->lwUsers->addItem(name);
    addToLog(name + " joined chat", Qt::black);
}

void Dialog::slotRemoveUser(QString name)
{
    for (int i = 0; i < ui->lwUsers->count(); ++i)
        if (ui->lwUsers->item(i)->text() == name)
        {
            ui->lwUsers->takeItem(i);
            addToLog(name + " left chat", Qt::black);
            break;
        }
}

void Dialog::slotNewMessage(QString& message, QString from, const QStringList &users)
{
    if (users.isEmpty())
        addToLog("Message from " + from + " to all" + ": " + message, Qt::darkBlue);
    else
    {
        addToLog("Message from " + from + " to " + users.join(", ") + ": "+ message, Qt::darkBlue);
    }
}

void Dialog::on_pbSend_clicked()
{
    QString message = ui->pteMessage->document()->toPlainText();
    if(message.contains(QRegExp("[^\\s]"))){
        if (ui->lwUsers->count() == 0)
        {
            QMessageBox::information(this, "", "No clients connected");
            return;
        }

        QStringList UserList = ui->pteAdresses->document()->toPlainText().split(QRegExp("[,.\\s]\\s*"), QString::SkipEmptyParts);
        foreach(QString s, UserList){
            if(!server->isNameUsed(s)){
                QMessageBox::information(this, "", "There is no clients under this name:" + s);
                return;
            }
        }
        emit sigMessageFromServer(message, UserList);

        ui->pteMessage->clear();
    }
}

void Dialog::on_pbStartStop_clicked()
{
    if (server -> isListening())
    {
        addToLog("Server stopped at " + server->serverAddress().toString() + "/" + QString::number(server->serverPort()), Qt::darkGreen);
        emit sigServerStop();
        ui->pbStartStop->setText("Start server");
    }
    else
    {
        QHostAddress addr;
        if (!addr.setAddress(ui->leHost->text()))
        {
            addToLog("Invalid address " + ui->leHost->text(), Qt::darkRed);
            return;
        }
        if (server->startServer(addr, ui->lePort->text().toInt()))
        {
            addToLog("Server started at " + ui->leHost->text() + "/" + ui->lePort->text(), Qt::darkGreen);
            ui->pbStartStop->setText("Stop server");
        }
        else
        {
            addToLog("Server can't start at " + ui->leHost->text() + "/" + ui->lePort->text(), Qt::darkRed);
            ui->pbStartStop->setText("Start server");
        }
    }
}

void Dialog::addToLog(QString text, QColor color)
{
    ui->lwLog->insertItem(0, QTime::currentTime().toString() + " " + text);
    ui->lwLog->item(0)->setForeground(color);
}

void Dialog::on_lwUsers_itemSelectionChanged()
{
    QStringList UserList;
    foreach (QListWidgetItem *i, ui->lwUsers->selectedItems()){
        UserList << i->text();
    }
    ui->pteAdresses->setPlainText(UserList.join("\n"));
}
