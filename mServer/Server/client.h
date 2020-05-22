#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QThreadPool>
#include <QtGui>
#include <QRegExp>
#include "myserver.h"
class MyServer;

class Client : public QObject
{
    friend class MyServer;
    Q_OBJECT

public:
    static const QString constNameUnknown;
    static const quint8 comAutchReq = 1;
    static const quint8 comUsersOnline = 2;
    static const quint8 comUserJoin = 3;
    static const quint8 comUserLeft = 4;
    static const quint8 comMessageToAll = 5;
    static const quint8 comMessageToUser = 6;
    static const quint8 comPublicServerMessage = 7;
    static const quint8 comPrivateServerMessage = 8;
    static const quint8 comAutchSuccess = 9;
    static const quint8 comDelivered = 10;
    static const quint8 comAudioRequest = 11;
    static const quint8 comAudioConfirm = 12;
    static const quint8 comAudioReject = 13;
    static const quint8 comErrNameInvalid = 201;
    static const quint8 comErrNameUsed = 202;

    explicit Client(int desc, MyServer *server, QObject *parent = 0);
    ~Client();
    void setName(QString name) {myName = name;}
    QString getName() const {return myName;}
    QString getPubKey() const {return userPublicKey;}
    bool getAuthed() const {return isAuthed;}
    void sendCommand(quint8 comm) const;                //отправить команду см.выше константы пользователю
    void sendUsersOnline() const;                       //отправить список пользователей

signals:
    void sigRemoveUser(Client *client);                 //сообщение об отключении пользователя
    void moreDataToRead();  //считалась не вся инф-ция

private slots:
    void slotConnect();     //установление соединения
    void slotDisconnect();  //разрыв соединения
    void slotReadyRead();   //доступна новая инф-ция
    void slotError(QAbstractSocket::SocketError socketError) const; //ошибка соединения

private:
    QTcpSocket *socket; // сокет соединения с клиентом
    MyServer *server;   // сервер с другими пользователями
    quint16 blockSize;  // размер блока для отправки сообщений
    QString myName;     // имя клиента
    bool isAuthed;      // статус авторизации клиента
    QString userPublicKey; // публичный ключ клиента

};

#endif // CLIENT_H
