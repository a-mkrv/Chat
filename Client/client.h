#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>
#include <QStackedLayout>
#include <QListWidgetItem>


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
    void getMessage();                  //Получение сообщений
    void on_sendMessage_clicked();      //Отправка сообщения
    void on_userStatus_clicked();       //Соединение к серверу (переработать)
    void show_Error(QAbstractSocket::SocketError errorSocket);  //Их большая часть в коде -_-
    void send_personal_data();          //Отправка сведений о клиенте
    void onDisconnect();                //Отключение
    void sendUserCommand(QString command);
    void on_userSetting_clicked();      //Настройки (-)

private:
    QStackedLayout *layout;
    Ui::Client *ui;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QString getIP();
    bool personDates;
    bool sethide;

};

#endif // CLIENT_H
