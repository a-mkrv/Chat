#include "user.h"

User::User()
{
    this->UserName = "Unknown";
    socket = nullptr;
}

void User::setUserName(QString name)
{
    this->UserName = name;
}

QString User::getUserName()
{
    return UserName;
}

void User::setSocket(QTcpSocket* socket)
{
    if (socket == nullptr)
        delete socket;
    this->socket = socket;
}

QTcpSocket* User::getSocket()
{
    return socket;
}


User::~User()
{

}

