#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QThreadPool>
#include <QtGui>
#include "/home/elias/Documents/Chat/mServer/Server/client.h"      //need for constants
#include <QMessageBox>
#include "mesdata.h"
#include <qrsaencryption.h>

class Client;

class MesData;

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    enum Status{
        statNoConnection = 0,
        statWaitAccept = 1,
        statIncomingCAll = 2,
        statCalling = 3
    };



private slots:
    void slotSocketConnected();     //произошло подключение к серверу
    void slotSocketDisconnected();  //произошло отключение от сервера
    void slotSocketReadyRead(); //появилась новая инф-ция для чтения
    void slotSocketDisplayError(QAbstractSocket::SocketError socketError);  //ошибка подключения

    void on_pbConnect_clicked();    //подключение к серверу
    void on_pbSend_clicked();   // отправка сообщения
    void pbAudio_clicked();
    void on_lwUsers_itemSelectionChanged(); //изменились выделенные пользователи

    void slotAudioPlay();


signals:
    void sigShiftEnterPushed();

private:
    //атрибуты окна и сервера
    Ui::Dialog *ui;  // интерфейс пользователя
    QTcpSocket *socket; //сокет для связи
    quint16 blockSize; // размер блока отправки сообщений
    QString myName; //собственное имя
    bool connected; //состояние связи с сервером

    //данные о других пользователях
    QMap<QString, QByteArray> KeyList;    //имена и публичные ключи шифрования пользователей
    QRSAEncryption  encrypt; // объект, задающий шифрование
    QByteArray pubKey, privKey; // наши ключи шифрования
    void updateUserList(QMap<QString, QString> users); //обновление списка пользоваьтелей (All всегда существует) по пришедшему от сервера списку

    //записи о событиях
    QMap<QString, QList<MesData> *> dmessages;  // история соообщений
    QList<MesData> *log; // лог подключений
    void addToLog(QString text, QColor color = Qt::black); //сообщения сервера
    void insertMessage(QString userName, QString sender, QString target, QString text, QTime time, QColor color = Qt::black, bool delivered = true); //добавление сообщения в историю

    // аудио
    QPushButton *pbAudio; // кнопка аудиовызова
    QString activeUser;
    QHostAddress activeUserAdress;
    quint16 activeUserPort;
    Status audioStatus;
    void enableAudio();
    void disableAudio();
    QUdpSocket *audioSok;  // сокет для аудиосвязи
    QHostAddress audioAdress;
    quint16 audioPort;
    QAudioInput* audioInput;
    QAudioOutput* audioOutput;
    QIODevice *deviceIn;
    QIODevice *deviceOut;
    void startAudioStream(QHostAddress &in_userAdress, quint16 in_userPort);
    void stopAudio();

    //методы
    void updateScreen(); //обновление экрана
    virtual bool eventFilter(QObject * target, QEvent *event);  //перехват Shift+Enter
};





#endif // DIALOG_H
