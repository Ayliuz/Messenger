#ifndef MYAUDIOENCRYPTDEVICE_H
#define MYAUDIOENCRYPTDEVICE_H

#include <QObject>
#include <QDebug>
#include <QThreadPool>
#include <QtGui>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QUdpSocket>
#include <qrsaencryption.h>

class AudioCryptDevice : public QObject
{
    Q_OBJECT

public:
    explicit AudioCryptDevice(QByteArray &in_userPublicKey, QByteArray &in_ourPrivateKey, QRSAEncryption &in_encrypt, QObject *parent);
    ~AudioCryptDevice();

    QHostAddress getAdress(){ return audioAdress;}
    quint16 getPort(){ return audioPort;}
    void startStream(QHostAddress &in_userAdress, quint16 in_userPort);

private:
        QUdpSocket *audioSok;  // сокет для аудиосвязи

        //аудиосвязь
        QHostAddress audioAdress;
        quint16 audioPort;
        QAudioInput* audioInput;
        QAudioOutput* audioOutput;
        QIODevice *deviceIn;
        QIODevice *deviceOut;

        QHostAddress userAdress;
        quint16 userPort;
        QByteArray userPubKey;
        QByteArray ourPrivKey;
        QRSAEncryption encrypt;

private slots:
        void slotAudioPlay();
        void slotPlay();
};

#endif // MYAUDIOENCRYPTDEVICE_H
