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
    registration.cpp \
    newcontact.cpp \
    chatlistdelegate.cpp \
    aboutdialog.cpp \
    notification.cpp \
    main.cpp \
    rsacrypt.cpp \
    hashmd5.cpp \
    confirmwindow.cpp \
    creategroup.cpp \
    userlistdelegate.cpp \
    choicecreate.cpp

HEADERS  += client.h \
    emojiframe.h \
    findcontacts.h \
    trayicon.h \
    tcpconnect.h \
    emojimanager.h \
    listwidgetemoji.h \
    registration.h \
    newcontact.h \
    chatlistdelegate.h \
    aboutdialog.h \
    notification.h \
    rsacrypt.h \
    hashmd5.h \
    confirmwindow.h \
    creategroup.h \
    userlistdelegate.h \
    choicecreate.h

FORMS    += client.ui \
    emojiframe.ui \
    findcontacts.ui \
    registration.ui \
    newcontact.ui \
    aboutdialog.ui \
    confirmwindow.ui \
    creategroup.ui \
    choicecreate.ui

RESOURCES += \
    image.qrc


