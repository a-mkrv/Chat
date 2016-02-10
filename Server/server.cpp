#include "server.h"
#include "ui_server.h"
#include <QDir>

Server::Server(QWidget *parent) :
    QMainWindow(parent), nextBlockSize(0),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    timer = new QTimer;
    sqlitedb = new SQLiteDB;
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 55155))
    {
        QMessageBox::critical(this, tr("Chat Server"),
                              tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        close();
        return;
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(NewConnect()));
    connect(this, SIGNAL(NewConnect()), this, SLOT(getMessage()));
}

void Server::NewConnect()
{
    User *newUser = new User;
    newUser->setSocket(tcpServer->nextPendingConnection());
    QTcpSocket *newSocket = newUser->getSocket();                 // Подключение нового клиента
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    clientConnections.append(newUser);

    qDebug() << "Новое соединение" << newSocket->socketDescriptor();

    QByteArray *buffer = new QByteArray();
    qint32 *s = new qint32(0);
    buffers.insert(newSocket, buffer);
    sizes.insert(newSocket, s);
}

void Server::onDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "Start Disconnect";

    User* disconnectedUser;
    for (auto i : clientConnections)
    {
        if (i->getSocket() == socket)
        {
            disconnectedUser = i;

            if(!ui->userList->size().isEmpty())
            {
                for(int j=0; j<ui->userList->count(); j++)
                    if(ui->userList->item(j)->data(Qt::DisplayRole)== disconnectedUser->getUserName())
                    {
                        ui->userList->removeItemWidget(ui->userList->takeItem(j));
                        ui->chatDialog->addItem(timeconnect() + " - " + disconnectedUser->getUserName() + " disconnected ");
                    }
            }
        }
    }
    clientConnections.removeAll(disconnectedUser);
}

void Server::sendToID(QString message, int ID)
{
    // - Отправка конкретному пользователю

    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << message;

    for (auto i : clientConnections)
        if (i->getSocket()->socketDescriptor() == ID)
            i->getSocket()->write(msg);
}

void Server::getMessage()
{
    QTime   time;
    QString typePacket;

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    QDataStream in(client);
    in.setVersion(QDataStream::Qt_5_4);

    if (!nextBlockSize) {
        if (quint16(client->bytesAvailable()) < sizeof(quint16)) {
            return;
        }
        in >> nextBlockSize;
    }

    if (client->bytesAvailable() < nextBlockSize) {
        return;
    }

    in >> time >> typePacket;
    qDebug() << typePacket;
    int command = 0;
    // 0 - пусто,
    // 1 - имя пользователя,
    // 2 - команда,
    // 3 - поиск
    // 4 - Файл
    // 5 - Регистрация

    if (typePacket == "_USR_")
        command = 1;

    else if (typePacket == "_UCD_")
        command = 2;

    else if (typePacket == "_FND_")
        command = 3;

    else if (typePacket == "_FILE_")
        command = 4;

    else if(typePacket == "_REG_")
        command = 5;

    else if(typePacket == "_LOG_IN_")
        command = 6;

    switch (command)
    {

    case 1:
    {
        QString newUser;
        in >> newUser;
        NewUser(client, newUser);
        break;
    }

    case 2:
    {
        QString newMessage;
        in >> newMessage;
        PrivateMessage(client, newMessage);
        break;
    }

    case 3:
    {
        QString findUser, whoFind;
        in >> findUser >> whoFind;
        QString result = sqlitedb->FindInDB(findUser, whoFind);

        if (result!="false")
        {
            sendToID("_FIN_ OKFIN " + result, client->socketDescriptor());
            sqlitedb->addChatTable(whoFind, findUser);
            ui->chatDialog->addItem(timeconnect() + " - " + whoFind + " added " + findUser );
        }
        else
            sendToID("_FIN_ NOFIN", client->socketDescriptor() );
        break;
    }

    case 4:
    {
        SendingFile(client);
        nextBlockSize = 0;
        break;
    }

    case 5:
    {
        QString UserName, City, Password, Age, Sex;
        in >> UserName >> City >> Password >> Age >> Sex;
        LogIn(client, UserName, City, Password, Age, Sex);

        break;
    }

    case 6:
    {
        qDebug() << "LOG";
        QString Login, Password;
        in >> Login >> Password;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        if(!sqlitedb->CorrectInput(Login, Password))
        {
            out << QString("Error_Login_Pass");
            client->write(block);
        }
        else
        {
            QString message = "LogInOK!";
            out << message;
            client->write(block);
        }

        break;
    }

    default:
        qDebug() << "Default";
    }
}

//void Server::sendToClient(QTcpSocket* mSocket, const QString& typePacket, QString report)
//{
//    QByteArray  arrBlock;
//    QDataStream out(&arrBlock, QIODevice::WriteOnly);

//    out.setVersion(QDataStream::Qt_4_7);
//    out << quint16(0) << QTime::currentTime() << typePacket << report;

//    out.device()->seek(0);
//    out << quint16(arrBlock.size() - sizeof(quint16));

//    mSocket->write(arrBlock);
//}



void Server::NewUser(QTcpSocket *client, QString _user)
{
    QString UserName = _user;
    QString TMPName = UserName;
    bool AlreadyName = true;
    int numInc = 0;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QList <QPair<QString, QString>> lst;
    ChatListVector chatlst;

    lst = sqlitedb->FriendList(UserName, chatlst);

    out << QString("FRLST") << lst << chatlst;
    client->write(block);

    while (AlreadyName)
    {
        AlreadyName = false;

        for (auto i : clientConnections)
            if (i->getUserName() == UserName)
                AlreadyName = true;

        if (AlreadyName)
        {
            TMPName = UserName + "(" + QString::number(numInc) + ")";
            ++numInc;
        }
    }

    UserName = TMPName;

    for (auto i : clientConnections)
        if (i->getSocket() == client)
        {
            i->setUserName(UserName);
            ui->userList->addItem(i->getUserName());
            ui->chatDialog->addItem(timeconnect() + " - " + i->getUserName() + " is online (" + QString::number(client->socketDescriptor()) + ")");
        }
}

void Server::PrivateMessage(QTcpSocket *client, QString _message)
{
    User* toUser = nullptr;
    User* fromUser = nullptr;

    QStringList WordList;
    QString Message = _message;

    WordList = Message.split(" ", QString::SkipEmptyParts);
    Message = WordList.takeFirst();

    for (auto i : clientConnections)
        if (i->getSocket() == client)
            fromUser = i;


    QString recipient = WordList.takeFirst();

    QString text;
    text.clear();
    for (auto i : WordList)
    {
        text += i;
        text += " ";
    }


    if (!WordList.isEmpty())
        for (auto i : clientConnections)
            if (i->getUserName() == recipient)
                toUser = i;

    QString newMessage = "*To: " + recipient + ": " + text;
    sendToID(newMessage, fromUser->getSocket()->socketDescriptor());

    if (toUser != nullptr)
    {
        newMessage.clear();
        newMessage = "*From: " + fromUser->getUserName() + ": " + text;
        sendToID(newMessage, toUser->getSocket()->socketDescriptor());

        ui->chatDialog->addItem(timeconnect() + " - PM: " + fromUser->getUserName() + " -> " + toUser->getUserName() + ":  " + text);
        sqlitedb->addMessInChat(fromUser->getUserName(), toUser->getUserName(), text, QString("To"));
        sqlitedb->addMessInChat(toUser->getUserName(), fromUser->getUserName(),  text, QString("From"));
    }
    else
    {
        sqlitedb->addMessInChat(fromUser->getUserName(), recipient, text, QString("To"));
        sqlitedb->addMessInChat(recipient, fromUser->getUserName(),  text, QString("From"));
    }
}

void Server::SendingFile(QTcpSocket *client)
{
    QTcpSocket* mClientSocket = (QTcpSocket*)sender();
    QDataStream in(mClientSocket);
    in.setVersion(QDataStream::Qt_5_4);

    QByteArray buffer;
    QString fileName;
    qint64 fileSize;

    QString dirDownloads = QDir::homePath() + "/op/";
    QDir(dirDownloads).mkdir(dirDownloads);

    in >> fileName >> fileSize;
    qDebug() << fileName;
    qDebug() << fileSize;

    forever
    {
        if (!nextBlockSize) {

            if (quint16(mClientSocket->bytesAvailable()) < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        in >> buffer;

        //sendToClient(mClientSocket, "SC_FILE_TRANSFER_ASK", fileName);

        if (mClientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
    }

    QFile receiveFile(dirDownloads + fileName);
    receiveFile.open(QIODevice::ReadWrite);
    receiveFile.write(buffer);
    receiveFile.close();
    buffer.clear();
    return;
    nextBlockSize = 0;
}


void Server::LogIn(QTcpSocket *client, QString &U, QString &C, QString &P, QString &A, QString &S)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    if(U.isEmpty() || P.isEmpty())
    {
        out << QString("PassEmpty");
        client->write(block);
    }
    else if(sqlitedb->FindInDB(U, 0)=="false")
    {
        if(C.isEmpty())
            C="Unknown";
        sqlitedb->AddContact(U, S, A.toInt(), C, P);
        ui->chatDialog->addItem(timeconnect() + " - User registration: " + U);
        qDebug() << "Новый пользователь: " << U;


        out << QString("Welcome!");
        client->write(block);
    }
    else
    {
        out << QString("Already!");
        client->write(block);
    }
}

QString Server::timeconnect()
{
    QString time = QDateTime::currentDateTime().toString();
    return "[" + time + "]";
}

void Server::Status()
{
    for (auto i : clientConnections)
        qDebug() << i->getUserName();
}

Server::~Server()
{
    delete ui;
}
