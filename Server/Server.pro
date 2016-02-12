#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T17:53:03
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        server.cpp \
    sqlitedb.cpp \
    user.cpp

HEADERS  += server.h \
    sqlitedb.h \
    user.h

FORMS    += server.ui
