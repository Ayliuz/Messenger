#include "mesdata.h"

MesData::MesData(Client *in_sender, Client *in_target, QString in_text, QTime in_time, QObject *parent = 0) : QObject(parent){
    sender = in_sender;
    target = in_target;
    text = in_text;
    time = in_time;
}

~MesData(){

}
