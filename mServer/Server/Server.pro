#-------------------------------------------------
#
# Project created by QtCreator 2011-10-13T18:08:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app

include($$PWD/../Qt-Secret/src/Qt-Secret.pri)

SOURCES += main.cpp\
    client.cpp \
    myserver.cpp \
    dialog.cpp

HEADERS  += \
    client.h \
    myserver.h \
    dialog.h

FORMS    += dialog.ui
