#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    personDates = "null";
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(show_Error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(send_personal_data()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

}

Client::~Client()
{
    delete ui;
}

void Client::on_sendMessage_clicked()
{


}

void Client::on_userStatus_clicked()
{
    QDataStream in(tcpSocket);
    QString hostname = "127.0.0.1";
    quint16 port = 55155;
    QString status = tr("-> Connecting to 127.0.0.1 on port 55155.");
    new QListWidgetItem(status, ui->userList);
    ui->chatDialog->scrollToBottom();
    tcpSocket->abort();
    tcpSocket->connectToHost(hostname, port);
}

void Client::getMessage()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);

    QString message;
    in >> message;

    QStringRef checkMessage(&message, 0, 5);
    int command = 0; // 1 = userlist

    if(checkMessage == "_LIST")
        command = 1;

    QStringList commandList;

    switch(command)
    {
      case 1:
        commandList = message.split(" ", QString::SkipEmptyParts);
        commandList.removeFirst();
        ui->userList->clear();

        for(auto i : commandList)
        {
            new QListWidgetItem(i, ui->userList);
        }

        break;
    default:
        new QListWidgetItem(message, ui->userList);
        ui->chatDialog->scrollToBottom();

    }
}


void Client::show_Error(QAbstractSocket::SocketError errorSocket)
{
    switch (errorSocket)
    {
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::information(this, tr("Chat Client"),
            tr("Disconnected from Server."));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Chat Client"),
            tr("The host was not found.\nPlease check the hostname and port settings."));
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Chat Client"),
                tr("The connection was refused by the peer.\n"
                "Make sure the server is running,\n"
                "and check that the host name and port\n"
                "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("Chat Client"),
                tr("The following error occurred: %1.").arg(tcpSocket->errorString()));
    }
}

void Client::send_personal_data()
{
    if(personDates!="null")
    {
        personDates = "Ok";

        new QListWidgetItem("Sending personal dates...", ui->chatDialog);
        ui->chatDialog->scrollToBottom();

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        QString command = "_USER_";
        QString username = "Anton";
        out << command;
        out << username;
        tcpSocket->write(block);
    }
}

void Client::onDisconnect()
{
    new QListWidgetItem("Disconnected..", ui->chatDialog);
    ui->chatDialog->scrollToBottom();
    ui->userList->clear();
    personDates = "null";
}
