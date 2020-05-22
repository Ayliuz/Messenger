#-------------------------------------------------
#
# Project created by QtCreator 2011-10-13T22:35:09
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Messenger
TEMPLATE = app

include(/home/elias/Documents/Chat/mMessenger/Qt-Secret/src/Qt-Secret.pri)

SOURCES += main.cpp\
        dialog.cpp \
        mesdata.cpp \


HEADERS  += \
    dialog.h \
    mesdata.h \


FORMS    += dialog.ui



