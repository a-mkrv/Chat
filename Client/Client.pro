#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T17:52:00
#
#-------------------------------------------------

QT       += core gui network quick
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        client.cpp \
    emojiframe.cpp \
    setting.cpp \
    findcontacts.cpp \
    trayicon.cpp \
    tcpconnect.cpp \
    emojimanager.cpp \
    listwidgetemoji.cpp \
    listdelegate.cpp \
    registration.cpp \
    newcontact.cpp \
    chatlistdelegate.cpp

HEADERS  += client.h \
    emojiframe.h \
    setting.h \
    findcontacts.h \
    trayicon.h \
    tcpconnect.h \
    emojimanager.h \
    listwidgetemoji.h \
    listdelegate.h \
    registration.h \
    newcontact.h \
    chatlistdelegate.h

FORMS    += client.ui \
    emojiframe.ui \
    findcontacts.ui \
    registration.ui \
    maingui.ui \
    newcontact.ui

RESOURCES += \
    image.qrc
