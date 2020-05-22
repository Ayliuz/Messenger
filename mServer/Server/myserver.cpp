#include "myserver.h"

MyServer::MyServer(QObject *parent) :QTcpServer(parent)
{
    encrypt = QRSAEncryption(QRSAEncryption::Rsa::RSA_256);
}

MyServer::~MyServer()
{

}

bool MyServer::startServer(QHostAddress addr, qint16 port)
{
    //Для запуска сервера нам необходимо вызвать в конструкторе метод listen() он принимает IP-адрес и порт
    //При возникновении ошибочных ситуаций, этот метод вернет false, на которые мы отреагируем и выведим окно
    //сообщения об ошибке
    if (!listen(addr, port))
    {
        qDebug() << "Server not started at" << addr << ":" << port;
        return false;
    }
    qDebug() << "Server started at" << addr << ":" << port;
    return true;
}

void MyServer::slotServerStop()
{
     close();
     foreach (Client* cl, clients) {
         cl->deleteLater();
     }
     clients.clear();
}

void MyServer::sendToAllUserJoin(QString name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    //резервируем 2 байта для размера блока и отправляем имя добавившегося и обновлённый список
    out << (quint16)0 << Client::comUserJoin << name << getUsersOnline();

    //возваращаемся в начало
    out.device()->seek(0);

    //вписываем размер блока на зарезервированное место
    out << (quint16)(block.size() - sizeof(quint16));

    //рассылаем всем авторизованным пользователям
    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getName() != name && clients.at(i)->getAuthed())
            clients.at(i)->socket->write(block);
    }

    //добавляем сообщение в лог
    emit sigAddUserToList(name);
}

void MyServer::sendToAllUserLeft(QString name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    //резервируем 2 байта для размера блока и отправляем имя отключившегося и обновлённый список
    out << (quint16)0 << Client::comUserLeft << name << getUsersOnline();
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    //рассылаем всем авторизованным пользователям
    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getName() != name && clients.at(i)->getAuthed())
            clients.at(i)->socket->write(block);

    //убираем из списка
    emit sigRemoveUserFromList(name);
}

void MyServer::sendToAllMessage(QString &message, QString fromUsername)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::comMessageToAll << fromUsername << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    //успешность передачи
    bool success = false;

    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getAuthed()){
            clients.at(i)->socket->write(block);
            success = true;
        }
    }

    //добавляем сообщение в лог, если отправка успешна
    if(success){
        emit sigMessageToLog(message, fromUsername, QStringList());
    }
}

void MyServer::sendToAllServerMessage(QString &message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::comPublicServerMessage << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    //успешность передачи
    bool success = false;

    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getAuthed()){
            clients.at(i)->socket->write(block);
            success = true;
        }
    }
    //добавляем сообщение в лог, если отправка успешна
    if(success){
        emit sigMessageToLog(message, "Server", QStringList());
    }
}

void MyServer::sendToUsersServerMessage(QString &message, const QStringList &users)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray encodeMessage; // зашифрованное сообщение

    //успешность передачи
    bool success = false;

    for (int j = 0; j < clients.length(); ++j){
        if (users.contains(clients.at(j)->getName())){

            // шифруем сообщение
            encodeMessage = encrypt.encode(message.toUtf8(), QByteArray::fromHex(clients.at(j)->userPublicKey.toUtf8()), encrypt.getRsa());

            //переводим сообщение в hex представление и в строку и так отправляем
            out << (quint16)0 << Client::comPrivateServerMessage << QString(encodeMessage.toHex());
            out.device()->seek(0);
            out << (quint16)(block.size() - sizeof(quint16));
            clients.at(j)->socket->write(block);
            success = true;

            //очищаем устройство ввода-вывода
            out.device()->seek(0);
            block.clear();
        }
    }

    //добавляем сообщение в лог, если отправка успешна
    if(success){
        emit sigMessageToLog(message, "Server", users);
    }
}

void MyServer::sendMessageToUser(QString &encodeMessage, const QString user, QString fromUsername)
{
    QByteArray block, blockToSender;
    QDataStream out(&block, QIODevice::WriteOnly);
    QDataStream outToSender(&blockToSender, QIODevice::WriteOnly);

    //успешность передачи пользователям адресатам и отправителю
    bool success = false;

    for (int j = 0; j < clients.length(); ++j){

        //отправляем сообщение отправителю только если оно от него же
        if (clients.at(j)->getName() == fromUsername && user == fromUsername){
            outToSender << (quint16)0 << Client::comMessageToUser << fromUsername << encodeMessage;
            outToSender.device()->seek(0);
            outToSender << (quint16)(blockToSender.size() - sizeof(quint16));
            clients.at(j)->socket->write(blockToSender);
            success = true;
        }

        //отправляем сообщение адресату
        else if (clients.at(j)->getName() == user){
            out << (quint16)0 << Client::comMessageToUser << fromUsername << encodeMessage;
            out.device()->seek(0);
            out << (quint16)(block.size() - sizeof(quint16));
            clients.at(j)->socket->write(block);
            success = true;
        }

    }

    //добавляем сообщение в лог, если отправка успешна
    if(success){
        QString message("Encrypted message");
        emit sigMessageToLog(message, fromUsername, QStringList(user));
    }
}

//отсылаем дальше подтверждение получения сообщения сообщение:(отправитель->сервер->получатель), подтверждение ровно наоборот
void MyServer::sendDelivered(QString user, QString fromUserName){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::comDelivered << user;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getName() == fromUserName){
            clients.at(i)->socket->write(block);
        }
    }
}

//отсылаем список пользователей и их ключей
QMap<QString, QString> MyServer::getUsersOnline() const
{
    QMap<QString, QString> UserKeyList;
    foreach (Client *c, clients){
        if (c->getAuthed())
            UserKeyList.insert(c->getName(), c->getPubKey());
    }
    return UserKeyList;
}

bool MyServer::isNameValid(QString name) const
{
    if (name.length() > 20 || name.length() < 3)
        return false;
    QRegExp r("[A-Za-z0-9_][$%*&@#!\\-A-Za-z0-9_]+");
    return r.exactMatch(name);
}

bool MyServer::isNameUsed(QString name) const
{
    QRegExp r("\\s*[aA]ll\\s*");
    if (r.exactMatch(name))
        return true;

    for (int i = 0; i < clients.length(); ++i)
        if (clients.at(i)->getName() == name)
            return true;
    return false;
}

void MyServer::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle, this, this);
    clients.append(client);

    connect(client, SIGNAL(sigRemoveUser(Client*)), this, SLOT(slotUserDisconnect(Client*)));
}

void MyServer::slotUserDisconnect(Client *client)
{
    //если клиент авторизован, удаляем его имя из списков, если нет - просто разрываем соединение
    if(client->isAuthed){
        QString name = client->myName;

        //убираем из вектора
        clients.remove(clients.indexOf(client));
        client->deleteLater();

        //сообщаем всем, что клиент вышел
        sendToAllUserLeft(name);
    }
    else{
        //убираем из вектора
        clients.remove(clients.indexOf(client));
        client->deleteLater();
    }
}

void MyServer::slotMessageFromServer(QString &message, const QStringList &users)
{
    if (users.isEmpty())
        sendToAllServerMessage(message);
    else
        sendToUsersServerMessage(message, users);
}



//аудиосвязь


void MyServer::sendAudioRequest(QString user, QString fromUser, QHostAddress &adress, quint16 port){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::comAudioRequest << fromUser << adress << port;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getName() == user && user != fromUser){
            clients.at(i)->socket->write(block);
        }
    }
}

void MyServer::sendAudioReject(QString user, QString fromUser){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << Client::comAudioReject << fromUser;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    for (int i = 0; i < clients.length(); ++i){
        if (clients.at(i)->getName() == user && user != fromUser){
            clients.at(i)->socket->write(block);
        }
    }
}

