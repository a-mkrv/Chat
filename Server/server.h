#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QFile>
#include <QPair>

#include "sqlitedb.h"
#include "user.h"


namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private slots:
    void getMessage();
    void NewConnect();
    void onDisconnect();
    void Status();

    void sendToID(QString message, int ID);     //Отправка личных сообщений
    void NewUser(QTcpSocket *client, QString _user);
    void PrivateMessage(QTcpSocket *client, QString _message, QString);
    void SendingFile(QTcpSocket *client);
    void LogIn(QTcpSocket *client, QString &U, QString &C, QString &P, QString &A, QString &S, QString &PubK, QString &Salt);

private:
    Ui::Server *ui;
    SQLiteDB *sqlitedb;
    QTcpServer *tcpServer;
    QList<User*> clientConnections;       //Список подключений

    QString timeconnect();                      //Время соединения
    quint32 nextBlockSize;
    quint32 nextBlockSize2;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    QTimer *timer;
};

#endif // SERVER_H
