#ifndef OUTPUTAUDIOCRYPTODEVICE_H
#define OUTPUTAUDIOCRYPTODEVICE_H

#include <QObject>
#include <QDebug>
#include <QThreadPool>
#include <QtGui>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QUdpSocket>
#include <qrsaencryption.h>

class OutputAudioCryptoDevice : public QObject
{
    Q_OBJECT

public:
    OutputAudioCryptoDevice(QHostAddress &in_userAdress, quint16 in_userPort, QByteArray &in_userPublicKey, QByteArray &in_ourPrivateKey, QRSAEncryption &in_encrypt, QObject *parent);
    ~OutputAudioCryptoDevice();

    QHostAddress getAdress(){ return audioAdress;}
    quint16 getPort(){ return audioPort;}

private:
        QUdpSocket *audioSok;  // сокет для аудиосвязи

        //аудиосвязь
        QHostAddress audioAdress;
        quint16 audioPort;
        QAudioFormat audioFormat;
        QAudioInput* audioInput;
        QAudioOutput* audioOutput;
        QIODevice *device;

        QHostAddress userAdress;
        quint16 userPort;
        QByteArray userPubKey;
        QByteArray ourPrivKey;
        QRSAEncryption encrypt;

public slots:
        void startStream();
        void slotSendEncrypt();
};

#endif // OUTPUTAUDIOCRYPTODEVICE_H
