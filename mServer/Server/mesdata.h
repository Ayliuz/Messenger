#ifndef MESDATA_H
#define MESDATA_H

#include <QObject>
#include <QDebug>
#include <QtGui>
#include <QRegExp>
#include <client.h>
class Client;

class MesData : public QObject
{
    Q_OBJECT
public:
    explicit MesData(Client *in_sender, Client *in_target, QString in_text, QTime in_time, QObject *parent = 0);
    ~MesData();
    QTime getTime() const {return time;}
    QString getText() const {return text;}
    Client* getSender() const {return sender;}
    Client* getTarget() const {return target;}

private:
    QTime time;
    QString text;
    Client* sender;
    Client* target;
};

#endif // MESDATA_H
