#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QList>


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
    void sendUserList();

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private:
    Ui::Server *ui;

    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientConnections;       //Список подключений
    std::map<const int, QString> userList;      //Список пользователей
    void updateStatus(QString message);
    void sendToID(QString message, int ID);     //Отправка личных сообщений
    QTcpSocket* getSocket(int ID);              //Соединение к клиенту
    QString getUsername(int ID);                //Получить ник пользователя
    QString timeconnect();                      //Время соединения
    void doCommand(QString command, int ID);
};

#endif // SERVER_H
