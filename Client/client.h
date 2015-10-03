#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>


namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private slots:
    void getMessage();
    void on_sendMessage_clicked();
    void on_userStatus_clicked();
    void show_Error(QAbstractSocket::SocketError errorSocket);
    void send_personal_data();
    void onDisconnect();

private:
    Ui::Client *ui;
    QString personDates;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QString getIP();

};

#endif // CLIENT_H
