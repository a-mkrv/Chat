#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QPixmap>

Client::Client(QWidget *parent) : QMainWindow(parent), ui(new Ui::Client)
{
    ui->setupUi(this);
    personDates = false;
    tcpSocket = new QTcpSocket(this);
    QPixmap pix;
    pix = QApplication::style()->standardPixmap(QStyle::SP_TrashIcon);
    QIcon pic(":/Client/apple.jpg");
    ui->label_3->setPixmap(pix);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(show_Error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(send_personal_data()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(ui->userSetting, SIGNAL(clicked()), this, SLOT(on_userSetting_clicked()));

}

void Client::on_sendMessage_clicked()
{
    QString message = ui->editText->text();
    ui->editText->clear();

    blockSize = 0;

    if (!message.isEmpty())
    {
        if (message.at(0) == '/')
        {
            sendUserCommand(message);
        }
        else
        {
            QByteArray msg;
            QDataStream out(&msg, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_4);

            out << message;
            tcpSocket->write(msg);
        }
    }
}

void Client::on_userStatus_clicked()
{
    QString hostname = "127.0.0.1";
    quint16 port = 55155;
    QString status = tr("-> Connecting to 127.0.0.1 on port 55155.");

    new QListWidgetItem(status, ui->chatDialog);
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

    enum class COMMAND { NONE, USERLIST};
    COMMAND cmd = COMMAND::NONE;

    QStringRef checkCmd(&message, 0, 5);
    if (checkCmd == "_LST_")
        cmd = COMMAND::USERLIST;

    QStringList commandList;
    switch (cmd)
    {
    case COMMAND::USERLIST:
          qDebug() << "LOL";
        commandList = message.split(" ", QString::SkipEmptyParts);
        commandList.removeFirst();
        ui->userList->clear();

         QListWidgetItem *q;
        for (auto i : commandList)
        {

            q = new QListWidgetItem(i, ui->userList);
            q->setSizeHint(QSize(0,65));
            q->setTextAlignment(10);

        }
        break;
    default:

qDebug() << "LOL(((";
        //in >> message;
       new QListWidgetItem(message, ui->chatDialog);

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
    if(!personDates)
    {
        personDates = true;

        new QListWidgetItem("Sending personal dates...", ui->chatDialog);
        ui->chatDialog->scrollToBottom();

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        QString command = "_USR_";
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
    personDates = false;
}

void Client::sendUserCommand(QString command)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    command = "_UCD_ " + command;
    out << command;

    tcpSocket->write(msg);
}

void Client::on_userSetting_clicked()
{
   //
}

Client::~Client()
{
    delete ui;
}

