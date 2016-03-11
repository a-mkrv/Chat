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
CONFIG += c++11

SOURCES +=\
        client.cpp \
    emojiframe.cpp \
    findcontacts.cpp \
    trayicon.cpp \
    tcpconnect.cpp \
    emojimanager.cpp \
    listwidgetemoji.cpp \
    listdelegate.cpp \
    registration.cpp \
    newcontact.cpp \
    chatlistdelegate.cpp \
    aboutdialog.cpp \
    notification.cpp \
    main.cpp \
    rsacrypt.cpp \
    hashmd5.cpp \
    confirmwindow.cpp

HEADERS  += client.h \
    emojiframe.h \
    findcontacts.h \
    trayicon.h \
    tcpconnect.h \
    emojimanager.h \
    listwidgetemoji.h \
    listdelegate.h \
    registration.h \
    newcontact.h \
    chatlistdelegate.h \
    aboutdialog.h \
    notification.h \
    rsacrypt.h \
    hashmd5.h \
    confirmwindow.h

FORMS    += client.ui \
    emojiframe.ui \
    findcontacts.ui \
    registration.ui \
    newcontact.ui \
    aboutdialog.ui \
    confirmwindow.ui

RESOURCES += \
    image.qrc


