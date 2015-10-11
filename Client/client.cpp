#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QPixmap>
#include <QIcon>


Client::Client(QWidget *parent) : QMainWindow(parent), ui(new Ui::Client)
{
    ui->setupUi(this);
    frameEmoji = new EmojiFrame();

    //Создание отдельного класса под Списки пользователей и настройки! (Сделать)
    //Завтра военка, черт(

    QPixmap back_to_menu(":/new/prefix1/Resource/double78.png");
    QIcon ButtonIcon(back_to_menu);

    QPixmap setting(":/new/prefix1/Resource/tool257.png");
    QIcon ButtonIcon2(setting);

    QPixmap add_contact(":/new/prefix1/Resource/square292.png");
    QIcon ButtonIcon3(add_contact);

    QPixmap send(":/new/prefix1/Resource/right-arrow6.png");
    QIcon ButtonIcon4(send);

    QPixmap search_mes(":/new/prefix1/Resource/magnifying glass57.png");

    ui->close_setting_button_2->setIcon(ButtonIcon);
    ui->close_setting_button_2->setIconSize(back_to_menu.rect().size()/2);

    ui->userSetting_button->setIcon(ButtonIcon2);
    ui->userSetting_button->setIconSize(setting.rect().size());

    ui->newContact_Button->setIcon(ButtonIcon3);
    ui->newContact_Button->setIconSize(add_contact.rect().size()/2);

    ui->sendMessage->setIcon(ButtonIcon4);
    ui->sendMessage->setIconSize(send.rect().size());

    ui->search_label->setPixmap(search_mes);


    personDates = false;
    ui->widget_2->hide();
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(show_Error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(send_personal_data()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(ui->userSetting_button, SIGNAL(clicked()), this, SLOT(on_userSetting_clicked()));
    connect(ui->close_setting_button_2, SIGNAL(clicked()), this, SLOT(on_close_setting_button_clicked()));
    connect(ui->userList_3, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(whisperOnClick(QListWidgetItem*)));

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

void Client::on_connect_button_clicked()
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
    qDebug() << message;
    enum class COMMAND { NONE, USERLIST};
    COMMAND cmd = COMMAND::NONE;

    QStringRef checkCmd(&message, 0, 5);
    if (checkCmd == "_LST_")
        cmd = COMMAND::USERLIST;


    QStringList commandList;
    QIcon pic(":/new/prefix1/Resource/profile5.png");
    switch (cmd)
    {
    case COMMAND::USERLIST:
    {

        commandList = message.split(" ", QString::SkipEmptyParts);
        commandList.removeFirst();
        ui->userList->clear();

         QListWidgetItem *q;

         //QWidget* w = new QWidget();
         //QHBoxLayout* vbl = new QHBoxLayout;
         //QLabel* lab_01 = new QLabel;
         //lab_01->setText("444");
         //vbl->addWidget(lab_01);
        // w->setLayout(vbl);
         //ui->userList->setLayout(vbl);


        for (auto i : commandList)
        {

            q = new QListWidgetItem(i, ui->userList);
            q->setSizeHint(QSize(0,65));
            q->setIcon(pic);

           // ui->userList->setItemWidget(q, w);
        }
        break;
}
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
   ui->widget_2->show();
   //qDebug() << ui->userList_3->item(1)->text();
}

void Client::on_close_setting_button_clicked()
{
    ui->userList_3->clearFocus();
    ui->userList_3->clearSelection();
    ui->stackedWidget->setCurrentIndex(0);
    ui->widget_2->hide();
}

void Client::whisperOnClick(QListWidgetItem* user)
{
    QString section = user->text();
    if (section == "Profile")
        ui->stackedWidget->setCurrentIndex(0);
    else if(section=="Interface")
        ui->stackedWidget->setCurrentIndex(1);
    else if (section == "Chat options")
        ui->stackedWidget->setCurrentIndex(2);
    else
        ui->stackedWidget->setCurrentIndex(3);

}

Client::~Client()
{
    delete ui;
}

void Client::on_pushButton_clicked()
{
     showEmoji();

}

void Client::showEmoji()
{
    QPoint p = QCursor::pos();
    frameEmoji->setGeometry(p.x()-250, p.y() -300, 300, 250);
    frameEmoji->show();

}

void Client::showFindCont()
{

    QPoint p = QCursor::pos();
    findcont->setGeometry(p.x() +380, p.y() +70, 320, 350);
    findcont->show();
}

void Client::on_newContact_Button_clicked()
{
    findcont = new findcontacts();
    showFindCont();
}
