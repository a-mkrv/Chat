#include "server.h"
#include "ui_server.h"
#include <QDir>

Server::Server(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::Server), nextBlockSize(0)
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
    QStringList user_list;

    qDebug() << "Disconnect";

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
                        sqlitedb->UpOnlineStatus("Offline", disconnectedUser->getUserName());
                        sqlitedb->FriendListName(disconnectedUser->getUserName(), user_list);
                        NotificationNetwork(disconnectedUser->getUserName(), user_list, 0);
                    }
            }
        }
    }
    clientConnections.removeAll(disconnectedUser);
}

void Server::SendResponseToID(QString message, int ID)
{
    // - Отправка конкретному пользователю

    for (auto i : clientConnections)
        if (i->getSocket()->socketDescriptor() == ID)
        {
            i->getSocket()->write(message.toUtf8());
            break;
        }
}

void Server::getMessage()
{
    QString time;
    QString typePacket;

    QStringList splitWords;

    QTcpSocket *clientSocket = static_cast<QTcpSocket *>(QObject::sender());

    //19 - for time
    //time = clientSocket->read(19);
    typePacket = clientSocket->read(4);

    //clientSocket->flush();
    //clientSocket->waitForBytesWritten(4000);

    int command = 0;

    if (typePacket == "LOAD")
        command = 1;

    else if (typePacket == "MESS")
        command = 2;

    else if (typePacket == "FIND")
        command = 3;

    else if (typePacket == "FILE")
        command = 4;

    else if (typePacket == "REGI")
        command = 5;

    else if (typePacket == "AUTH")
        command = 6;

    else if (typePacket == "CLNH" || typePacket == "DELF")
        command = 7;

    else if (typePacket == "UINF")
        command = 8;

    else if (typePacket == "NGRP")
        command = 9;

    else if (typePacket == "GETI")
        command = 10;

    switch (command)
    {

    case 1:
    {
        QString newUser;
        newUser = clientSocket->readAll();
        userIsOnline(clientSocket, newUser);

        break;
    }

    case 2:
    {
        QString newMessage, MyMsg;

        splitWords = requestSeparation(clientSocket->readAll());

        if (splitWords.size() == 2)
        {
            newMessage = splitWords[0];
            MyMsg = splitWords[1];
        }

        PrivateMessage(clientSocket, newMessage, MyMsg);

        break;
    }

    case 3:
    {
        qDebug() << "FIND";
        QString findUser, whoFind;

        splitWords = requestSeparation(clientSocket->readAll());

        if (splitWords.size() == 2)
        {
            findUser = splitWords[0];
            whoFind = splitWords[1];
        }

        QString result = sqlitedb->FindInDB(findUser, whoFind);

        if (result!="false")
        {
            // FNDP - Find Positive
            // FNDN - Find Negative
            SendResponseToID("FNDP" + result, clientSocket->socketDescriptor());
            sqlitedb->addChatTable(whoFind, findUser);
            sqlitedb->addChatTable(findUser, whoFind);  // Инвайт
            sqlitedb->FindInDB(whoFind, findUser);      // инвайт
            ui->chatDialog->addItem(timeconnect() + " - " + whoFind + " and " + findUser + " are now friends" );

            for (auto i : clientConnections)
                if (i->getUserName() == findUser)
                {
                    result = sqlitedb->FindInDB(whoFind, 0);
                    SendResponseToID("INVT" + whoFind + " /s " + result, i->getSocket()->socketDescriptor());
                    break;
                }

        }
        else
            SendResponseToID("FNDN", clientSocket->socketDescriptor() );

        break;
    }

    case 4:
    {
        SendingFile(clientSocket);
        nextBlockSize = 0;

        break;
    }

    case 5:
    {
        QString userName, city, password, age, sex, publicKey, salt;

        splitWords = requestSeparation(clientSocket->readAll());
        qDebug() << splitWords;

        if (splitWords.size() == 7)
        {
            userName = splitWords[0];
            city = splitWords[1];
            password = splitWords[2];
            age = splitWords[3];
            sex = splitWords[4];
            publicKey = splitWords[5];
            salt = splitWords[6];
        }

        LogIn(clientSocket, userName, city, password, age, sex, publicKey, salt);

        break;
    }

    case 6:
    {
        QString login, password;
        splitWords = requestSeparation(clientSocket->readAll());

        if (splitWords.size() == 2)
        {
            login = splitWords[0];
            password = splitWords[1];
        }
        qDebug() << login << password;
        QString result_return = sqlitedb->CorrectInput(login, password);
        qDebug() << result_return;

        if (result_return == "false")
        {   // Error Auth = Error login / password
            clientSocket->write(QString("ERRA").toUtf8());
        }
        else
        {
            clientSocket->write(QString("OKEY").toUtf8());
            clientSocket->write(result_return.toUtf8());
        }

        break;
    }
    case 7:
    {
        QString from, to;

        splitWords = requestSeparation(clientSocket->readAll());

        if (splitWords.size() == 2)
        {
            from = splitWords[0];
            to = splitWords[1];
        }

        if (typePacket == "CLNH")
            sqlitedb->ClearHistory(from, to);

        else if (typePacket == "DELF")
            sqlitedb->delFriend(from, to);

        break;
    }

    case 8:
    {
        QString  userInfo;
        QStringList dataList;

        userInfo = clientSocket->readAll();
        dataList = sqlitedb->UserData(userInfo);

        //FIXME
        //Send List insted string
        //clientSocket->write(QString("UINF" + dataList).toUtf8());

        clientSocket->write(QString("UINF").toUtf8());

        break;
    }
    case 9:
    {
        QStringList userList;
        QString groupName, groupDescr;

        //FIXME
        //in >> groupName >> groupDescr >> userList;
        sqlitedb->createGroup(groupName, groupDescr, userList);

        break;
    }

    case 10:
    {
        QString user;
        user = clientSocket->readAll();

        QString userInfo = sqlitedb->getFullUserInformations(user);
        clientSocket->write(userInfo.toUtf8());
    }
    }
}

QStringList Server::requestSeparation(QString text)
{
    QString outStr =  text;
    QStringList list = outStr.split(" /s ");

    return list;
}

void checkString(QString &response)
{
    if (response.length() > 4)
    {
        response = response.left(response.length() - 4);
        response.append(" //s ");
    }
}

void Server::userIsOnline(QTcpSocket *client, QString _user)
{
    QString response = "FLST";
    QString UserName = _user;
    QString TMPName = UserName;

    bool AlreadyName = true;
    int numInc = 0;

    QStringList SendMyStatus;
    ChatListVector ChatFriendList;
    PairStringList friendOnlineStatus;
    PairStringList friendsList;
    PairStringList publicFriendKeys;

    sqlitedb->UpOnlineStatus("Online", UserName);
    sqlitedb->getOnlineStatus(UserName, friendOnlineStatus, SendMyStatus);
    publicFriendKeys = sqlitedb->FriendKeys(UserName);
    friendsList = sqlitedb->FriendList(UserName, ChatFriendList);

    int countUsers = friendsList.size();

    if (countUsers > 0) {
        for (int i = 0; i < countUsers; i++) {
            response.append(publicFriendKeys.at(i).first + " _ " + publicFriendKeys.at(i).second + " /s ");
        }

        checkString(response);

        for (int i = 0; i < countUsers; i++) {
            response.append(friendsList.at(i).first + " _ " + friendsList.at(i).second + " /s ");
        }

        checkString(response);

        ////////////

        //FIXME
        //MESSAGES

        ////////////

        for (int i = 0; i < countUsers; i++) {
            response.append(friendOnlineStatus.at(i).first + " _ " + friendOnlineStatus.at(i).second + " /s ");
        }

        checkString(response);

        qDebug() << response;
        client->write(response.toUtf8());
    }

    //FIXME
    //NotificationNetwork(UserName, SendMyStatus, 1);

    while (AlreadyName)
    {
        AlreadyName = false;

        for (auto i : clientConnections)
        {
            if (i->getUserName() == UserName)
            {
                AlreadyName = true;
                break;
            }
        }

        if (AlreadyName)
        {
            TMPName = UserName + "(" + QString::number(numInc) + ")";
            ++numInc;
        }
    }

    UserName = TMPName;

    for (auto i : clientConnections)
    {
        if (i->getSocket() == client)
        {
            i->setUserName(UserName);
            ui->userList->addItem(i->getUserName());
            ui->chatDialog->addItem(timeconnect() + " - " + i->getUserName() + " is online (" + QString::number(client->socketDescriptor()) + ")");
            break;
        }
    }
}

void Server::NotificationNetwork(const QString username, const QStringList &friend_list, int state)
{
    QStringList online_now;
    QString message;

    if (state == 1)
    {
      message = "STON" + username;
    }
    else if (state == 0)
    {
       message = "STOF" + username;
    }

    for (int i = 0; i < ui->userList->count(); i++)
        online_now.push_back(ui->userList->item(i)->text());

    QSet<QString> whom_alert = friend_list.toSet().intersect(online_now.toSet());

    for (auto i : clientConnections)
    {
        for (int j = 0; j < whom_alert.size(); j++)
        {
            if (whom_alert.contains(i->getUserName()))
            {
                SendResponseToID(message, i->getSocket()->socketDescriptor());
                break;
            }
        }
    }
}

void Server::PrivateMessage(QTcpSocket *client, QString _message, QString _mymsg)
{
    User* toUser = nullptr;
    User* fromUser = nullptr;

    QStringList WordList;
    QString Message = _message;

    WordList = Message.split(" ", QString::SkipEmptyParts);
    Message = WordList.takeFirst();

    for (auto i : clientConnections)
    {
        if (i->getSocket() == client)
        {
            fromUser = i;
            break;
        }
    }

    QString recipient = WordList.takeFirst();
    QString text;

    text.clear();
    for (auto i : WordList)
    {
        text += i;
        text += " ";
    }


    if (!WordList.isEmpty())
    {
        for (auto i : clientConnections)
        {
            if (i->getUserName() == recipient)
            {
                toUser = i;
                break;
            }
        }
    }

    QString newMessage = "*To: " + recipient + ": " + _mymsg;
    SendResponseToID(newMessage, fromUser->getSocket()->socketDescriptor());

    if (toUser != nullptr)
    {
        newMessage.clear();
        newMessage = "*From: " + fromUser->getUserName() + ": " + text;
        SendResponseToID(newMessage, toUser->getSocket()->socketDescriptor());

        ui->chatDialog->addItem(timeconnect() + " - PM: " + fromUser->getUserName() + " -> " + toUser->getUserName() + ":  " + text);
        sqlitedb->addMessInChat(fromUser->getUserName(), toUser->getUserName(), _mymsg, QString("To"));
        sqlitedb->addMessInChat(toUser->getUserName(), fromUser->getUserName(),  text, QString("From"));
    }
    else
    {
        sqlitedb->addMessInChat(fromUser->getUserName(), recipient, _mymsg, QString("To"));
        sqlitedb->addMessInChat(recipient, fromUser->getUserName(),  text, QString("From"));
    }
}

void Server::SendingFile(QTcpSocket *client)
{
    // Overwrite send files
    QTcpSocket* mClientSocket = (QTcpSocket*)sender();
    QDataStream in(mClientSocket);
    in.setVersion(QDataStream::Qt_5_4);

    QByteArray buffer;
    QString fileName;
    QString receiver_name;
    QString from_name;
    qint64 fileSize;

    in >> receiver_name >> fileName >> fileSize;
    qDebug() << receiver_name;
    qDebug() << fileName;
    qDebug() << fileSize;

    QString dirDownloads = QDir::homePath() + "/Whisper File Srorage/" + receiver_name + "/";
    QDir(dirDownloads).mkdir(dirDownloads);

    QThread::sleep(1);

    forever
    {
        if (!nextBlockSize) {

            if (quint32(mClientSocket->bytesAvailable()) < sizeof(quint32)) {
                break;
            }
            in >> nextBlockSize;
        }

        in >> buffer;


        for (auto i : clientConnections)
            if (i->getSocket() == client)
                from_name = i->getUserName();

        for (auto i : clientConnections)
            if (i->getUserName() == receiver_name)
            {

                QByteArray  arrBlock;
                QDataStream out(&arrBlock, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_5_4);

                out << quint32(0) << QString("GETF") << from_name
                    << fileName << fileSize << buffer;

                out.device()->seek(0);
                out << quint32(arrBlock.size() - sizeof(quint32));

                i->getSocket()->write(arrBlock);
            }
        if (mClientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
    }


    ui->chatDialog->addItem(timeconnect() + " - PM: " + from_name + " -> " + receiver_name + ":  " + "FILE " + fileName);
    sqlitedb->addMessInChat(from_name, receiver_name, "FILE " + fileName, QString("To") + " " + QString::number((float)fileSize/1024, 'f', 2)  + " KB");
    sqlitedb->addMessInChat(receiver_name, from_name,  "FILE " + fileName, QString("From") + " " + QString::number((float)fileSize/1024, 'f', 2)  + " KB");

    QFile receiveFile(dirDownloads + fileName);
    receiveFile.open(QIODevice::ReadWrite);
    receiveFile.write(buffer);
    receiveFile.close();
    buffer.clear();

    nextBlockSize = 0;
}


void Server::LogIn(QTcpSocket *client, QString &U, QString &C, QString &P, QString &A, QString &S, QString &PubK, QString &Salt)
{
    if(U.isEmpty() || P.isEmpty())
    {
        // Password is empty (pass nill)
        client->write(QString("PNIL").toUtf8());
    }
    else if (sqlitedb->FindInDB(U, 0) == "false")
    {
        if (C.isEmpty())
            C = "Unknown";

        sqlitedb->AddContact(U, S, A.toInt(), C, P, PubK, Salt);
        ui->chatDialog->addItem(timeconnect() + " - User registration: " + U);
        qDebug() << "Новый пользователь: " << U;

        client->write(QString("WELC").toUtf8());
    }
    else
    {
        client->write(QString("ALRD").toUtf8());
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
