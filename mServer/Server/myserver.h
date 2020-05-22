#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QDebug>
#include "client.h"
#include <qrsaencryption.h>
class Client;

class MyServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MyServer(QObject *parent = 0);
    ~MyServer();
    bool startServer(QHostAddress addr, qint16 port);                   //запуск listen()

    void sendToAllUserJoin(QString name);                               //сообщить о новом пользователе
    void sendToAllUserLeft(QString name);                               //сообщить об удалении старого пользователя
    void sendToAllMessage(QString &message, QString fromUsername);      //отправить общее сообщение
    void sendToAllServerMessage(QString &message);                      //отправить публичное сообщение сервера
    void sendToUsersServerMessage(QString &message, const QStringList &users);  //отправить личное сообщение сервера
    void sendMessageToUser(QString &encodeMessage, const QString user, QString fromUsername);            //отправить peer-to-peer
    void sendDelivered(QString user, QString fromUserName);             //подтвердить доставку сообщения
    void sendAudioRequest(QString user, QString fromUser, QHostAddress &adress, quint16 port);        //запрос на аудизвонок
    void sendAudioReject(QString user, QString fromUser);         //завершить или отменить вызов

    QMap<QString, QString> getUsersOnline() const;                      //отправить пользователю список пользователей и их публичные ключи
    bool isNameValid(QString name) const;                               //проверка на валидность имени
    bool isNameUsed(QString name) const;                                //проверка на использование имени и имя не должно быть All - название общего чата

signals:
    void sigRemoveUserFromList(QString name);                           //отправить ui сигнал об удалении пользователя
    void sigAddUserToList(QString name);                                //отправить ui сигнал о добавлении пользователя
    void sigMessageToLog(QString &message, QString from, const QStringList &users);     //новое сообщение в серверный лог

public slots:
    void slotMessageFromServer(QString &message, const QStringList &users); //отправить сообщение от сервера
    void slotUserDisconnect(Client *client);                                //действия при отключении пользователя
    void slotServerStop();                                                  //отключение сервера

protected:
    void incomingConnection(qintptr handle);                                //установление нового соединения

private:
    QVector<Client *> clients;
    QRSAEncryption  encrypt; // объект, задающий шифрование

};


#endif // MYSERVER_H
