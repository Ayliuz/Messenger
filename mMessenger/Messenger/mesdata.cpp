#include "mesdata.h"

MesData::MesData(const QString& in_sender, const QString& in_target, const QString& in_text, QTime in_time, QColor in_color = Qt::black, bool in_delivered = true) {
    sender = in_sender;
    target = in_target;
    text = in_text;
    time = in_time;
    color = in_color;
    delivered = in_delivered;
}

void MesData::setDelivered(bool state){
    delivered = state;
}

MesData:: ~MesData(){

}
