#ifndef USER_H
#define USER_H

#include <QTcpSocket>
#include <QHostAddress>

class User
{
public:
    User();
    ~User();

public slots:
    void setUserName(QString name);
    void setSocket(QTcpSocket* socket);

    QTcpSocket* getSocket();
    QString getUserName();

private:
    QString UserName;
    QTcpSocket* socket;

};

#endif // USER_H
