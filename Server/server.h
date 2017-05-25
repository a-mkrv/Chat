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

typedef QList <QPair <QString, QString>> PairStringList;


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

    void SendResponseToID(QString message, int ID);     //Отправка личных сообщений
    void userIsOnline(QTcpSocket *client, QString _user);
    void privateMessage(QTcpSocket *client, QStringList);
    void SendingFile(QTcpSocket *client);
    void LogIn(QTcpSocket *client, QString &U, QString &C, QString &P, QString &A, QString &S, QString &PubK, QString &Salt);
    void NotificationNetwork(const QString, const QStringList &, int);
    QStringList requestSeparation(QString text);

    void on_pushButton_clicked();

private:
    Ui::Server *ui;
    SQLiteDB *sqlitedb;
    QTcpServer *tcpServer;
    QList<User*> clientConnections;       //Список подключений

    QString timeconnect();                      //Время соединения
    quint32 nextBlockSize;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    QTimer *timer;
};

#endif // SERVER_H
