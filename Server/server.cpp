#include "server.h"
#include "ui_server.h"
#include <QDir>

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 55155))
    {
        QMessageBox::critical(this, tr("Chat Server"),
            tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        close();
        return;
    }

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
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(sendUserList()));
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
    if (socket != 0)
    {
        std::vector<int> currentSockets;
        int socketID = 0;

        for (auto i : clientConnections)
            currentSockets.push_back(i->socketDescriptor());


        for (auto i : userList)
        {
            bool found = true;
            for (auto ii : currentSockets)
                if (i.first == ii)
                    found = false;

            if (found == true)
                socketID = i.first;

        }

        QString username = getUsername(socketID);

        auto iter = userList.find(socketID);
        if (iter != userList.end())
            userList.erase(iter);

        ui->userList->clear();
        for (auto i : userList)
            new QListWidgetItem(i.second, ui->userList);


        clientConnections.removeAll(socket);
        socket->deleteLater();
        updateStatus("Connection terminated. (" + username + ":" + QString::number(socketID) + ")");
    }
}

void Server::doCommand(QString command, int ID)
{
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

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    QDataStream in(client);
    in.setVersion(QDataStream::Qt_5_4);

    QString message;
    static int t=0;
    if (t<1)
    in >> message;
t++;

    QStringList messageTokens;
    //messageTokens = message.split(" ", QString::SkipEmptyParts);



    int command = 0; //0 - пусто, 1 - имя пользователя, 2 - команда, 3 - поиск

    if (message == "_USR_")
        command = 1;
//    if (messageTokens.at(0) == "_FND_")
//    {
//        find_User=messageTokens.at(1);
//        qDebug() << find_User;
//        command = 3;
//    }

//    if (messageTokens.at(0) == "_UCD_")
//        command = 2;

//    if(messageTokens.at(0) == "_FILE_")
//        command=4;

    switch (command)
    {
    case 1:
    {
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
        messageTokens.removeFirst();
        message.clear();
        for (auto i : messageTokens)
        {
            message += i;
            message += " ";
        }
        doCommand(message, client->socketDescriptor());
        break;
    }
     case 3:
    {   QString dat;
        for (auto i : userList)
        {
            dat = i.second;
               qDebug() << "i.Second= " << i.second;

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

    }

    case 4:
    {

        QByteArray *buffer = buffers.value(client);
           qint32 *s = sizes.value(client);
           qint32 size = *s;
           while (client->bytesAvailable() > 0)
           {
               buffer->append(client->readAll());
               qDebug() << buffer;
               while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) //While can process data, process it
               {
                   if (size == 0 && buffer->size() >= 4) //if size of data has received completely, then store it on our global variable
                   {
                       size = ArrayToInt(buffer->mid(0, 4));
                       *s = size;
                       buffer->remove(0, 4);
                   }
                   if (size > 0 && buffer->size() >= size) // If data has received completely, then emit our SIGNAL with the data
                   {
                       QByteArray data = buffer->mid(0, size);
                       buffer->remove(0, size);
                       size = 0;
                       *s = size;
                       //emit dataReceived(data);
                   }
               }
           }
    }
    default:

        QByteArray buffer;
        QString fileName;
        qint64 fileSize;

        QString dirDownloads = QDir::homePath() + "/meraLOL/";
        QDir(dirDownloads).mkdir(dirDownloads);

        in >> fileName >> fileSize;

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

               // if (mClientSocket->bytesAvailable() < nextBlockSize) {
                    break;
                }
            }

            QFile receiveFile(dirDownloads + fileName);
            receiveFile.open(QIODevice::ReadWrite);
            receiveFile.write(buffer);
            receiveFile.close();
            buffer.clear();

//        std::map<int, QString>::iterator it;
//        it = userList.find(client->socketDescriptor());
//        updateStatus("MSG: (" + it->second + ") " + message);

//        it = userList.find(client->socketDescriptor());
//        QString usr = it->second;
//        message = usr + ": " + message;
    }
}


qint32 Server::ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

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
    //qDebug() << userlist;
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
