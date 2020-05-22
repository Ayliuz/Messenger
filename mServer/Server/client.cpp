#include "client.h"
#include <QWidget>
#include <QMessageBox>

const QString Client::constNameUnknown = QString(".Unknown");

Client::Client(int desc, MyServer *serv, QObject *parent) :QObject(parent)
{
    server = serv;
    isAuthed = false;
    myName = constNameUnknown;
    userPublicKey = QByteArray(0);

    blockSize = 0;
    socket = new QTcpSocket(this);

    //устанавливаем дескриптор из incomingConnection()
    socket->setSocketDescriptor(desc);

    //подключаем сигналы
    connect(socket, SIGNAL(connected()), this, SLOT(slotConnect()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(moreDataToRead()), this, SLOT(slotReadyRead()));

}

Client::~Client()
{

}

void Client::slotConnect()
{
    //never calls, socket already connected to the tcpserver
    //we just binding to this socket here: socket->setSocketDescriptor(desc);
}

void Client::slotDisconnect()
{
    emit sigRemoveUser(this);
}

void Client::slotError(QAbstractSocket::SocketError socketError) const
{
    //w нужна для обсвобождения памяти от QMessageBox (посредством *parent = &w)
    QWidget w;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(&w, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(&w, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(&w, "Error", "The following error occurred: "+socket->errorString());
    }
    //тут вызовутся деструктор w и соответственно QMessageBox (по правилам с++)
}

void Client::slotReadyRead()
{
    QDataStream in(socket);

    //если считываем новый блок первые 2 байта это его размер
    if (blockSize == 0) {

        //если пришло меньше 2 байт ждем пока будет 2 байта
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;

        //считываем размер (2 байта)
        in >> blockSize;
        //qDebug() << myName << "blockSize now " << blockSize;
    }
    //qDebug() << myName << "bytes available:" << socket->bytesAvailable();

    //ждем пока блок прийдет полностью
    if (socket->bytesAvailable() < blockSize)
        return;
    else
        //можно принимать новый блок
        blockSize = 0;

    //3 байт - команда серверу
    quint8 command;
    in >> command;
    //qDebug() << myName << "Received command " << command;

    //для неавторизованный пользователей принимается только команда "запрос на авторизацию"
    if (!isAuthed && command != comAutchReq)
        return;

    switch(command)
    {
        case comAutchReq:{                                   //запрос на авторизацию
            QString name, publicKey;                         //считываем имя и публичный ключ
            in >> name >> publicKey;
            if (!server->isNameValid(name)){                 //проверяем имя
                sendCommand(comErrNameInvalid);              //отправляем ошибку
                return;
            }

            if (server->isNameUsed(name)){                   //проверяем не используется ли имя
                sendCommand(comErrNameUsed);                 //отправляем ошибку
                return;
        }
            myName = name;                                   //авторизация пройдена
            userPublicKey = publicKey;
            isAuthed = true;
            sendUsersOnline();                               //отправляем новому клиенту список активных пользователей
            server->sendToAllUserJoin(myName);               //сообщаем всем про нового ползователя
            break;
        }


        //от текущего пользователя пришло сообщение для всех
        case comMessageToAll:{
            QString message;
            in >> message;
            //отправляем его всем
            server->sendToAllMessage(message, myName);
            break;
        }

        //от текущего пользователя пришло сообщение для некоторых пользователей
        case comMessageToUser:
        {
            QString user;
            in >> user;
            QString message;
            in >> message;
            server->sendMessageToUser(message, user, myName);
            break;
        }

        case comDelivered:
        {
            QString user;
            in >> user;
            server->sendDelivered(myName, user);
            break;
        }

        case comAudioRequest:
        {
            QString user;
            QHostAddress fromAdress;
            quint16 port;
            in >> user >> fromAdress >> port;
            server->sendAudioRequest(user, myName, fromAdress, port);
            break;
        }

         case comAudioReject:
        {
            QString user;
            in >> user;
            server->sendAudioReject(user, myName);
            break;
        }
    }

    //если остались ещё блоки, считаем их той же функцией, отправив сигнал
    emit moreDataToRead();
}

void Client::sendCommand(quint8 comm) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comm;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    //qDebug() << "Send to" << myName << "command:" << comm;
}

void Client::sendUsersOnline() const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comUsersOnline;
    QMap<QString, QString> UserKeylist = server->getUsersOnline();
    out << UserKeylist;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    //qDebug() << "Send user list to" << myName << ":" << UserKeylist;
}


