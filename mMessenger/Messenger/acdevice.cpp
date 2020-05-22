#include "acdevice.h"

AudioCryptDevice::AudioCryptDevice(QByteArray &in_userPublicKey, QByteArray &in_ourPrivateKey, QRSAEncryption &in_encrypt, QObject *parent) : QObject(parent)
{
    userPubKey = in_userPublicKey;
    ourPrivKey = in_ourPrivateKey;
    encrypt = in_encrypt;

    audioSok = new QUdpSocket(this);
    audioAdress = QHostAddress::LocalHost;
    audioPort = 1234;
}

AudioCryptDevice::~AudioCryptDevice(){
    if(audioInput->state() == QAudio::State::ActiveState)
        audioInput->stop();

    if(audioOutput->state() == QAudio::State::ActiveState)
        audioOutput->stop();
}

//начало передачи аудио на определённый порт
void AudioCryptDevice::startStream(QHostAddress &in_userAdress, quint16 in_userPort){

    audioSok->bind(audioAdress, audioPort);
    connect(audioSok, SIGNAL(readyRead()), this, SLOT(slotPlay()));

    QAudioFormat audioFormatOut;
    audioFormatOut.setSampleRate(128000);
    audioFormatOut.setChannelCount(1);
    audioFormatOut.setSampleSize(16);
    audioFormatOut.setCodec("audio/pcm");
    audioFormatOut.setByteOrder(QAudioFormat::LittleEndian);
    audioFormatOut.setSampleType(QAudioFormat::UnSignedInt);

    //если формат не поддерживается, задаём ближайший поддерживающийся
    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());
    if (!infoOut.isFormatSupported(audioFormatOut))
        audioFormatOut = infoOut.nearestFormat(audioFormatOut);

    audioOutput = new QAudioOutput(audioFormatOut, this);
    deviceOut = audioOutput->start();

    userAdress = in_userAdress;
    userPort = in_userPort;

    QAudioFormat audioFormatIn;
    audioFormatIn.setSampleRate(128000);
    audioFormatIn.setChannelCount(1);
    audioFormatIn.setSampleSize(16);
    audioFormatIn.setCodec("audio/pcm");
    audioFormatIn.setByteOrder(QAudioFormat::LittleEndian);
    audioFormatIn.setSampleType(QAudioFormat::UnSignedInt);

    //если формат не поддерживается, задаём ближайший поддерживающийся
    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(audioFormatIn))
        audioFormatIn = infoIn.nearestFormat(audioFormatIn);

    audioInput = new QAudioInput(audioFormatIn, this);
    audioSok->connectToHost(userAdress, userPort);
    audioInput->start(audioSok);
}


void AudioCryptDevice::slotPlay(){
    while (audioSok->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(audioSok->pendingDatagramSize());
        audioSok->readDatagram(data.data(), data.size());
        deviceOut->write(data.data(), data.size());
    }
}
