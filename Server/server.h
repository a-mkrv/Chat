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

#include "sqlitedb.h"

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

private slots:
    void sendMessage(QString message, QTcpSocket& socket);
    void getMessage();
    void newConnection();
    void onDisconnect();
    void Status();

signals:
    void SEND_UserList();

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private:
    Ui::Server *ui;
    SQLiteDB *sqlitedb;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientConnections;       //Список подключений
    std::map<const int, QString> userList;      //Список пользователей
    void updateStatus(QString message);
    void sendToID(QString message, int ID);     //Отправка личных сообщений
    QTcpSocket* getSocket(int ID);              //Соединение к клиенту
    QString getUsername(int ID);                //Получить ник пользователя
    QString timeconnect();                      //Время соединения
    void doCommand(QString command, int ID);
    quint16 nextBlockSize;
    QString find_User;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely
    QTimer *timer;
};

#endif // SERVER_H
