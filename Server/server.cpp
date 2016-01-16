#include "server.h"
#include "ui_server.h"
#include <QDir>

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    nextBlockSize=0;
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 55155))
    {
        QMessageBox::critical(this, tr("Chat Server"),
            tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        close();
        return;
    }
    sqlitedb = new SQLiteDB;
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SIGNAL(SEND_UserList()));
    //timer->start(6000);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, SIGNAL(newConnection()), this, SLOT(getMessage()));
    connect(this, SIGNAL(SEND_UserList()), this, SLOT(sendUserList()));

}

void Server::newConnection()
{
    QTcpSocket *newSocket = tcpServer->nextPendingConnection();                 // Подключение нового клиента
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    //connect(newSocket, SIGNAL(disconnected()), this, SLOT(sendUserList()));
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    clientConnections.append(newSocket);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray *buffer = new QByteArray();
    qint32 *s = new qint32(0);
    buffers.insert(newSocket, buffer);
    sizes.insert(newSocket, s);

    QString message = "Server: Connected!";
    out << message;
    newSocket->write(block);
}

void Server::onDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString username = 0;
    if (socket != 0)
    {
        std::vector<int> currentSockets;
        int socketID = 0;

        for (auto i : clientConnections)
            currentSockets.push_back(i->socketDescriptor());

if(!userList.empty())
{
        for (auto i : userList)
        {
            bool found = true;
            for (auto ii : currentSockets)
                if (i.first == ii)
                    found = false;

            if (found == true)
                socketID = i.first;

        }

        username = getUsername(socketID);

        auto iter = userList.find(socketID);
        if (iter != userList.end())
            userList.erase(iter);

        ui->userList->clear();
        for (auto i : userList)
            new QListWidgetItem(i.second, ui->userList);
}

        clientConnections.removeAll(socket);
        socket->deleteLater();
        updateStatus("Connection terminated. (" + username + ":" + QString::number(socketID) + ")");
    }
}

void Server::doCommand(QString command, int ID)
{
    qDebug() << command;

    QString message = "Server: ";
    QStringList instruction = command.split(" ", QString::SkipEmptyParts);
    command = instruction.takeFirst();


    if (command == "/msg"  || command == "/pm")
    {
        if (!instruction.isEmpty())
        {
            QString recipient = instruction.takeFirst(); // удаляет первый элемент списка и возвращает его
            int rID = 0;

            for (auto i : userList)
                if (i.second == recipient)
                    rID = i.first;

            if (rID == 0)
                message += tr("User \"%1\" not found").arg(recipient);
            else if (rID == ID)
                message += "You cannot message yourself.";
            else
            {
                QString text;
                text.clear();
                for (auto i : instruction)
                {
                    text += i;
                    text += " ";
                }

                auto user = userList.find(ID);

                message = "* To: " + recipient + ": " + text;
                QString rMessage = "* From: " + user->second + ": " + text;
                sendToID(rMessage, rID);

                QString status = "PM: (" + user->second + " -> " + recipient + ") " + text;
                new QListWidgetItem(status, ui->chatDialog);
            }
        }
        else
            message = "*** Error: Incorrect " + command + " syntax.\n" + "*** Use: " + command + "[username] [message]";
    }

    else
           message += "Invalid command";

    sendToID(message, ID);
}

void Server::sendMessage(QString message, QTcpSocket& socket)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << message;
    socket.write(msg);
}

void Server::sendToID(QString message, int ID)
{
    // - Отправка конкретному пользователю

    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << message;

    for (auto i : clientConnections)
        if (i->socketDescriptor() == ID)
            i->write(msg);
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
    qDebug() << client->socketDescriptor();
    QString message;
    int command = 0; // 0 - пусто,
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
    {
         in >> find_User;
         command = 3;
    }

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
        qDebug() << "Case 1";
        in >> message;

        QString username = message;
        QString tempname = username;

        int ID = client->socketDescriptor();

        int numInc = 0;
        bool isTaken = true;

        while (isTaken)
        {
            isTaken = false;
            for (auto i : userList)
                if (i.second == tempname)
                {
                    isTaken = true;
                    ++numInc;
                }

            if (isTaken)
                tempname = username + "(" + QString::number(numInc) + ")";
        }

        username = tempname;

        userList.insert(std::make_pair(ID, username));
        new QListWidgetItem(username, ui->userList);
        ui->userList->scrollToBottom();

        QString address = getSocket(ID)->peerAddress().toString();
        QStringList addr = address.split(":", QString::SkipEmptyParts);
        address = addr.takeLast();

        QString newConnectionMsg = "New connection established. (" + username + ":" + QString::number(ID) + "->" + address + ")";
        updateStatus(newConnectionMsg);
        break;
    }

    case 2:
    {
        qDebug() << "Case 2";
        QStringList messageTokens;
        QString command_mes;
        in >> command_mes;

        messageTokens = command_mes.split(" ", QString::SkipEmptyParts);
        command_mes.clear();
        for (auto i : messageTokens)
        {
           command_mes += i;
           command_mes += " ";
        }
        doCommand(command_mes, client->socketDescriptor());
        break;
    }
     case 3:
    {
        qDebug() << "Case 3";

        QString dat;
        for (auto i : userList)
        {
            dat = i.second;

               if(find_User!=dat)
                    sendToID("_FIN_ NOFIN", client->socketDescriptor() );

               else if (find_User==dat)
             {
                QString recipient = find_User;
                int rID = 0;

                for (auto i : userList)
                    if (i.second == recipient)
                       rID = i.first;

                auto user = userList.find(client->socketDescriptor());
                            message = "_INV_ " + user->second;
                            qDebug() << message << "\n\n";
                            sendToID(message, rID);

                sendToID("_FIN_ OKFIN", client->socketDescriptor() );
                return;
            }
        }
        break;
    }

    case 4:
    {
        qDebug() << "Case 4";

               QByteArray buffer;
               QString fileName;
               qint64 fileSize;

               QString dirDownloads = QDir::homePath() + "/op/";
               QDir(dirDownloads).mkdir(dirDownloads);

               in >> fileName >> fileSize;
                qDebug() << fileName;
                qDebug() << fileSize;
               QMessageBox messageBox;
               messageBox.setInformativeText(QObject::trUtf8("Принять файл ") + fileName +
               QObject::trUtf8(" от ")  + "?");
               messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
               int ret = messageBox.exec();


               if (ret == QMessageBox::Save) {

                   forever
                    {
                        if (!nextBlockSize) {

                            if (quint16(client->bytesAvailable()) < sizeof(quint16)) {
                                break;
                            }
                            in >> nextBlockSize;
                        }

                        in >> buffer;

                        //sendToClient(mClientSocket, "SC_FILE_TRANSFER_ASK", fileName);

                        if (client->bytesAvailable() < nextBlockSize) {
                            break;
                        }
                    }

                    QFile receiveFile(dirDownloads + fileName);
                    receiveFile.open(QIODevice::ReadWrite);
                    receiveFile.write(buffer);
                    receiveFile.close();
                    buffer.clear();
                    break;
               }
               else
               {

               }
               nextBlockSize = 0;
break;
    }
    case 5:
    {
        qDebug() << "5";
        QString UserName;
        QString City;
        QString Password;
        QString Age;
        QString Sex;

        in >> UserName;
        in >> City;
        in >> Password;
        in >> Age;
        in >> Sex;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        if(!sqlitedb->FindInDB(UserName))
        {
        sqlitedb->AddContact(UserName, Sex, Age.toInt(), City, Password);
        qDebug() << "New" << UserName;

        QString message = "Welcome!";
        out << message;
        client->write(block);
        }
        else
        {
            QString message = "Already!";
            out << QString("Reg") << message;
            client->write(block);
        }

break;
    }
    case 6:
    {
        qDebug() << "6";
        QString Login;
        QString Password;

        in >> Login;
        in >> Password;
           qDebug() << Login << Password;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        if(!sqlitedb->CorrectInput(Login, Password))
        {
        QString message = "Error_Login_Pass";
        out << QString("LogIN") <<  message;
        client->write(block);
        }
        else
        {
            QString message = "Error_Login_Pass";
            out << message;
            client->write(block);
        }



        break;
    }
    default:
        qDebug() << "Default";
        std::map<int, QString>::iterator it;
        it = userList.find(client->socketDescriptor());
        updateStatus("MSG: (" + it->second + ") " + typePacket);
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

void Server::updateStatus(QString message)
{
    message = timeconnect() + " " + message;
    new QListWidgetItem(message, ui->chatDialog);
    ui->chatDialog->scrollToBottom();
}

void Server::sendUserList()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QString userlist;

    userlist = "_LST_";

    for (auto i : userList)
    {
        userlist += " ";
        userlist += i.second;
    }

    out << userlist;
    for (auto i : clientConnections)
        i->write(block);
}

QTcpSocket* Server::getSocket(int ID)
{
    QTcpSocket* socket;

    for (auto i : clientConnections)
        if (i->socketDescriptor() == ID)
            socket = i;

    return socket;
}

QString Server::getUsername(int ID)
{
    auto itr = userList.find(ID);
    return itr->second;
}

QString Server::timeconnect()
{
    QString time = QDateTime::currentDateTime().toString();
    return "[" + time + "]";
}

Server::~Server()
{
    delete ui;
}
