#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtGui>
#include <QtCore>
#include "myserver.h"

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    MyServer *server;   //сервер мессенджера
    void addToLog(QString text, QColor color);  //добавить в лог сервера сообщение

signals:
    void sigMessageFromServer(QString& message, const QStringList &users);  //новое сообщение для сервера на отправку пользователям от имени сервера
    void sigServerStop();   //отключить сервер

public slots:
    void slotAddUser(QString name);         //добавить пользователя в список
    void slotRemoveUser(QString name);      //удалить пользователя из списка
    void slotNewMessage(QString& message, QString from, const QStringList &users);  //новое сообщение

private slots:
    void on_pbStartStop_clicked();
    void on_pbSend_clicked();
    void on_lwUsers_itemSelectionChanged(); //изменился выбор адресатов
};

#endif // DIALOG_H
