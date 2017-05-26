#include "client.h"
#include "ui_client.h"
#include <time.h>
#include "client_define.h"

/****************************************************************/
/*                                                              */
/*   The main class that performs most of the work application. */
/*                Receiving a request from the server,          */
/*                    install the necessary settings,           */
/*                        and opening windows.                  */
/*                                                              */
/****************************************************************/


QString gl_fname;

Client::Client(QWidget *parent) : QMainWindow(parent), ui(new Ui::Client), personDates(false), download_path("(C:/...)")
{
  srand((time(NULL)));
  ui->setupUi(this);
  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

#ifdef __APPLE__
  RemoveSelections();
#endif

  frameEmoji      = new EmojiFrame();
  emojiMan        = new EmojiManager();
  notice          = new Notification();
  authorization   = new Authorization();
  trayIcon        = new TrayIcon(this);
  stackchat       = new QStackedWidget;
  trayIconMenu    = new QMenu(this);
  tcpSocket       = new QTcpSocket(this);
  rsaCrypt        = new RSACrypt;
  set_language    = new XML_Language();


  QColor defaulcolor(Qt::white);
  colorchat = defaulcolor.name();

  U_RB_sendEnter->setChecked(true);
  U_userList->setContextMenuPolicy(Qt::CustomContextMenu);
  U_stackedWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
  U_userList->setItemDelegate(new UserListDelegate(U_userList));
  U_search_list->setItemDelegate(new UserListDelegate(U_search_list));
  U_label_6->setText(QDir::homePath() + "/Whisper/");
  U_widget_2->hide();
  U_search_list->hide();
  U_glass_button->raise();
  U_glass_button->hide();
  SetDefaultLanguage();

  nextBlockSize = 0;
  FriendCount = 0;

  trayIconMenu->addAction(U_actionShowHideWindow);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(U_actionExit);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->hide();

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(IconActivated(QSystemTrayIcon::ActivationReason)));
  connect(authorization, SIGNAL(sendData(QString, QString, QString)), this, SLOT(RecieveData(QString, QString, QString)));
  connect(frameEmoji, SIGNAL(sendEmoji(QString)), this, SLOT(InsertEmoticon(QString)));

  connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(GetMessage()));
  connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ShowError(QAbstractSocket::SocketError)));
  connect(tcpSocket, SIGNAL(connected()), this, SLOT(SendPersonalData()));
  connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnect()));

  connect(U_userSetting_button, SIGNAL(clicked()), this, SLOT(on_userSetting_clicked()));
  connect(U_close_setting_button_2, SIGNAL(clicked()), this, SLOT(on_close_setting_button_clicked()));
  connect(U_userList_3, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(WhisperOnClick(QListWidgetItem*)));
  connect(U_userList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(WhisperOnClickUsers(QListWidgetItem*)));
  //connect(U_userList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(WhisperOnClickUsers(QListWidgetItem*)));
  connect(U_actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(U_stackedWidget_2, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(ShowContextMenuForChat(const QPoint &)));
  connect(U_userList, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(ShowContextMenuForWidget(const QPoint &)));

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

void Client::RecieveData(QString str, QString pas, QString pubKey)
{
  if (str != "" && pas != "")
    {
      name = str;
      name.replace(" ", "_");
      U_usernameEdit->setText(name);

      QDir keyDir = HOME_PATH_KEY;
      QLineEdit m_ptxtMask("*.txt");
      QStringList listFiles = keyDir.entryList(m_ptxtMask.text().split(" "), QDir::Files);
      myPublicKey = pubKey;
      myPrivateKey = "1187 3683"; //temporarily

      for (int i = 0; i < listFiles.size(); i++)
        if (listFiles.at(i) == name+".txt")
          {
            QFile myKey(HOME_PATH_KEY + name + ".txt");
            myKey.open(QIODevice::ReadOnly);
            //myPrivateKey = myKey.readAll(); // need a solution
            break;
          }

      QString hostname = "127.0.0.1";
      quint32 port = 55155;
      tcpSocket->abort();
      tcpSocket->connectToHost(hostname, port);

      this->show();
      QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
      animation->setDuration(1500);
      animation->setStartValue(0);
      animation->setEndValue(1);
      animation->start();

      trayIcon->show();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Sending a personal message
void Client::on_sendMessage_clicked()
{
  QRegExp rx("<a [^>]*name=\"([^\"]+)\"[^>]*>");       // Regular expressions to parse emoticons in a message
  QString str = U_editText->text();                    // Getting the message for sending
  QString message = str;

  QStringList list;
  QString encodemsg;

  list.clear();
  int pos = 0;

  while ((pos = rx.indexIn(str, pos)) != -1) {
      list << rx.cap(1);
      pos += rx.matchedLength();
    }

  for (int i = 0; i < list.count(); i++) {
      QString emojiNumber = list.at(i);
      QString searchTag = QString("<a name=\"%1\"></a>").arg(emojiNumber);
      QString replTag = emojiMan->getEmojiSymbolFromNumber(emojiNumber);
      str = str.replace(searchTag, replTag);
    }

  QTextDocument doc;
  doc.setHtml(U_textEdit1->toPlainText());
  doc.toPlainText();

  U_editText->clear();
  blockSize = 0;

  if (!message.isEmpty() && vec.size()!=0)              // Sending messages
    {
      QListWidgetItem *item = new QListWidgetItem();
      item->setData(Qt::UserRole + 1, "TO");
      item->setData(Qt::DisplayRole, message);
      item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
      chatvec.at(U_stackedWidget_2->currentIndex())->addItem(item);
      chatvec.at(U_stackedWidget_2->currentIndex())->scrollToBottom();

      if (U_ChBox_PSound->isChecked())
        QSound::play(SOUND_TO);

      QString RecName = vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString();
      QStringList lst = myPublicKey.split(" ", QString::SkipEmptyParts);
      QString MyMsg = rsaCrypt->EncodeText(message, lst.at(0).toInt(), lst.at(1).toInt());

      if (pubFriendKey.keys().contains(RecName))
        {
          QString user_key = pubFriendKey[RecName];
          qDebug() << "Friend key: " << user_key;

          QStringList _key = user_key.split(" ", QString::SkipEmptyParts);
          encodemsg = rsaCrypt->EncodeText(message, _key.at(0).toInt(), _key.at(1).toInt());
        }

      QString new_mes = "MESG" + U_usernameEdit->text() + " /s " + RecName + " /s " + MyMsg + " /s " + encodemsg;

      tcpSocket->write(new_mes.toUtf8());

    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Insert emoticon in message
void Client::InsertEmoticon(QString symbol)
{
  QTextCursor cursor(U_textEdit1->textCursor());

  if (!cursor.isNull()) {
      cursor.insertHtml(symbol);
    }

  QTextCursor cursor2(U_textEdit1->document()->find(symbol));

  QString emojiNumber = emojiMan->getEmojiNumberFromSymbol(symbol);
  QString binDir = QCoreApplication::applicationDirPath();
  QString dataDir = binDir;
  dataDir = QDir::cleanPath(dataDir + "/");

  QDir iconsLocation(dataDir + "/data/icons");


  QString s = QString("%1/x-%2.png")
      .arg(iconsLocation.absolutePath())
      .arg(emojiNumber);

  if (!cursor2.isNull()) {
      QString imgTag = QString("<img src=\"%1\" id=\"%2\" />")
          .arg(s)
          .arg(emojiNumber);

      cursor2.insertHtml(imgTag);
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Parsing and adding dialog to chat lists
void Client::AddChatToList(QString response)
{
  QStringList dialogues = response.split(" //s ");
  dialogues.removeFirst();
  QStringList prKeys = myPrivateKey.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < dialogues.size(); ++i)
    {
      if (dialogues[i].contains(" /pm ", Qt::CaseInsensitive))
        {
          QStringList messages = dialogues[i].split(" /pm ");
          QString userName = messages[0];
          messages.removeFirst();

          for (int j = 0; j < messages.size(); ++j)
            {
              QStringList msg = messages[j].split(" /s ");

              for (int k = 0; k < U_userList->count(); ++k)
                {
                  if ((vec.at(k)->data(Qt::DisplayRole).toString()) == userName)
                    {
                      QListWidgetItem *item2 = new QListWidgetItem();

                      if (msg[0] == "To")
                        {
                          msg[1] = rsaCrypt->DecodeText(msg[1], prKeys.at(0).toInt(), prKeys.at(1).toInt());

                          item2->setData(Qt::UserRole + 1, "TO");
                          item2->setData(Qt::DisplayRole, msg[1]); //text
                          item2->setData(Qt::ToolTipRole, msg[2]); //time
                          U_userList->item(k)->setData(Qt::UserRole + 1, "You: " + msg[1]);
                        }
                      else if (msg[0] == "From")
                        {
                          QStringList _key = pubFriendKey[userName].split(" ", QString::SkipEmptyParts);

                          msg[1] = rsaCrypt->DecodeText(msg[1], _key.at(0).toInt(), _key.at(1).toInt());

                          item2->setData(Qt::UserRole + 1, "FROM");
                          item2->setData(Qt::DisplayRole, msg[1]); //text
                          item2->setData(Qt::ToolTipRole, msg[2]); //time
                          U_userList->item(k)->setData(Qt::UserRole + 1, msg[1]);
                        }

                      U_userList->item(k)->setData(Qt::ToolTipRole, msg[2]);

                      chatvec.at(k)->addItem(item2);
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Adding users to contact list
void Client::AddUserToList(QString _username, QString _sex, PairStringList lst, int count)
{
  int rand_avatar = 0;
  QIcon pic;
  QString first_letter = _username.at(0).toLower();
  QString RU_Full_Path = RU_AVATARS_PATH + first_letter +".jpg";
  QString EN_Full_Path = EN_AVATARS_PATH + first_letter +".jpg";

  SexOfUsers.insert(_username, _sex);
  if (QFile::exists(EN_Full_Path))
    {
      pic.addFile(EN_Full_Path);
    }
  else if (QFile::exists(RU_Full_Path))
    {
      pic.addFile(RU_Full_Path);
    }
  else {
      if (_sex == "Man")
        rand_avatar = rand()%18+1;
      else if (_sex == "Woman")
        rand_avatar = rand()%13+19;
      else if (_sex == "Unknown")
        rand_avatar = rand()%20+32;
      pic.addFile(":/Avatars/Resource/Avatars/"+QString::number(rand_avatar)+".jpg");
    }

  // Check the reinclusion person on the list - return.
  if (!vec.empty())
    for (int i = 0; i < vec.size(); i++)
      if (vec.at(i)->data(Qt::DisplayRole) == _username)
        return;

  //Create a list on each page stack to display a separate correspondence and chat through a delegate managed property Item'a
  QListWidgetItem *item = new QListWidgetItem();
  QListWidget *chatlist = new QListWidget();
  chatlist->setItemDelegate(new ChatListDelegate(chatlist, colorchat));
  chatlist->setAttribute(Qt::WA_MacShowFocusRect, false);
  U_stackedWidget_2->addWidget(chatlist);

  // Adding a new user through the search.
  // count - just flag
  if (count == -1 || count == -2)
    {
      item->setData(Qt::DisplayRole, _username);
      item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
      if (count == -2)
        {   QListWidgetItem *item2 = new QListWidgetItem();
          item->setData(Qt::UserRole + 1, lan_dict.value("Ad_User"));
          item2->setData(Qt::UserRole + 1, "FROM");
          item2->setData(Qt::DisplayRole, lan_dict.value("Invate_msg"));
          item2->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
          chatlist->addItem(item2);
        }
      else
        item->setData(Qt::UserRole + 1, "New User");
      item->setData(Qt::DecorationRole, pic);

      vec.push_back(item);
      chatvec.push_back(chatlist);
      U_userList->addItem(item);
      U_userList->scrollToBottom();

      if (count == -2)
        {
          U_start_textBrowser->hide();
          U_stackedWidget_2->show();
          U_stackedWidget_2->setCurrentIndex(chatvec.size()-1);
          U_userList->clearSelection();
          vec.at(vec.size()-1)->setSelected(true);
          WhisperOnClickUsers(vec.at(vec.size()-1));
        }
      return;
    }

  // Adding a user (friend) from the database
  item->setData(Qt::DisplayRole, _username);
  item->setData(Qt::ToolTipRole, FriendOnlineStatus[_username]);
  item->setData(Qt::DecorationRole, pic);
  vec.push_back(item);
  chatvec.push_back(chatlist);

  // Loading chat history.
  // F - From, T - To
  for (int i = 0; i < lst.size(); i++)
    {
      QListWidgetItem *item2 = new QListWidgetItem();
      QString msg = lst.at(i).first;
      QString timeStr = (lst.at(i).first);
      timeStr.remove(14, lst.at(i).first.size());

      msg.remove(0,14);
      QString TFile = msg.left(4);

      if (lst.at(i).second.left(1) == "F")
        {
          QStringList lstfrom = myPrivateKey.split(" ", QString::SkipEmptyParts);

          if (msg.left(1) == "F")
            {

            }
          else
            msg = rsaCrypt->DecodeText(msg, lstfrom.at(0).toInt(), lstfrom.at(1).toInt());

          if (TFile == "FILE")
            {
              msg.remove(0,5);
              QString fsize = lst.at(i).second;
              QIcon pic(ICON_FILE_PATH);
              item2->setData(Qt::ToolTipRole, fsize.remove(0,5) +  "  " +timeStr);
              item2->setData(Qt::UserRole + 1, "FROMF");
              item2->setData(Qt::DecorationRole, pic);
              item->setData(Qt::UserRole + 1, lan_dict.value("File") + ": " + msg);
            }
          else
            {
              item2->setData(Qt::UserRole + 1, "FROM");
              item2->setData(Qt::ToolTipRole, timeStr);
              item->setData(Qt::UserRole + 1, msg);
            }

          item2->setData(Qt::DisplayRole, msg);
          item->setData(Qt::ToolTipRole, timeStr);

        }
      else if (lst.at(i).second.left(1) == "T")
        {
          if (msg.left(1) == "F")
            {

            }
          else{
              QStringList _lst = myPrivateKey.split(" ", QString::SkipEmptyParts);
              msg = rsaCrypt->DecodeText(msg, _lst.at(0).toInt(), _lst.at(1).toInt());
            }

          if (TFile == "FILE")
            {
              msg.remove(0,5);
              QString fsize = lst.at(i).second;
              QIcon pic(ICON_FILE_PATH);
              item2->setData(Qt::ToolTipRole, fsize.remove(0,3) + "  " + timeStr);
              item2->setData(Qt::UserRole + 1, "TOF");
              item2->setData(Qt::DecorationRole, pic);
            }
          else
            {
              item2->setData(Qt::UserRole + 1, "TO");
              item2->setData(Qt::ToolTipRole, timeStr);
            }

          item2->setData(Qt::DisplayRole, msg);
        }

      chatvec.at(count)->addItem(item2);
    }
  chatvec.at(count)->scrollToBottom();
  U_userList->clearSelection();
  U_stackedWidget_2->hide();
  U_userList->addItem(item);

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Parse response from server - Users Info. Adding through Client::AddUserToList(...)
void Client::ParseResponseUserList(QString response, ChatListVector &chatList)
{
  QString userName;
  PairStringList tmp; // Fix - remove this.
  QStringList keyList;
  QString res = response;
  QStringList dataList = res.split(" /s ");
  dataList.removeLast();

  for (int i = 0; i < dataList.size(); ++i)
    {
      QStringList dataSet = dataList[i].split(" _ ");
      userName = dataSet[0];

      keyList = dataSet[2].split(" ");
      pubFriendKey[userName] = keyList.at(0) + " " + keyList.at(1);
      FriendOnlineStatus[userName] = dataSet[4];

      //TODO: The sex of a person is not indicated temporarily.
      AddUserToList(userName, "Man", tmp, i);
    }

  // Request for load chat history per users
  if (dataList.size() > 0)
    {
      QString request = "LCHT";
      request.append(U_usernameEdit->text());

      tcpSocket->write(request.toUtf8());
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Parse response from server - User Chats. Adding through Client::AddChatToList(...)
void Client::ParseResponseChatList(QString )
{

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Receiving a message from a user / server
void Client::GetMessage()
{
  QString typePacket;
  QString message;

  typePacket = tcpSocket->read(4);
  message = tcpSocket->readAll();
  // qDebug() << typePacket <<  message;
  //  if (!nextBlockSize) {
  //      if (quint32(tcpSocket->bytesAvailable()) < sizeof(quint32)) {
  //          return;
  //        }
  //      in >> nextBlockSize;
  //    }
  //  if (tcpSocket->bytesAvailable() < nextBlockSize) {
  //      return;
  //  }

  QStringList commandList;
  QString fromname;

  // Parsing of an incoming message.

  enum class COMMAND { NONE, USERLIST, USERMSG, NEWMSG, FINDUSER, INVITE, GETFILE, USINFO, ISONLINE};
  COMMAND cmd = COMMAND::NONE;

  if (typePacket == "FLST")
    cmd = COMMAND::USERLIST;

  else if (typePacket == "EXST" || typePacket == "EMPT")
    cmd = COMMAND::USERMSG;

  else if (typePacket == "GETF") // don't work currently
    cmd = COMMAND::GETFILE;

  else if (typePacket == "INFP" || typePacket == "INFN")
    cmd = COMMAND::USINFO;

  else if (typePacket == "FNDP" || typePacket == "FNDN")
    cmd = COMMAND::FINDUSER;

  else if (typePacket == "INVT")
    cmd = COMMAND::INVITE;

  else if (typePacket == "NMSG")
    cmd = COMMAND::NEWMSG;

  else if (typePacket == "STON" || typePacket == "STOF")
    cmd = COMMAND::ISONLINE;

  if (typePacket != "FLST")
    {
      commandList = message.split(" /s ");

      if (commandList.at(0) == name)
        return;

      fromname = commandList.at(0);
    }

  switch (cmd)
    {

    // Friends List, keys
    case COMMAND::USERLIST:
      {
        ChatListVector chatList;
        ParseResponseUserList(message, chatList);

        break;
      }

      // Receiving a list of messages
    case COMMAND::USERMSG:
      {
        AddChatToList(message);

        break;
      }

      // Response to a request on the search friend
    case COMMAND::FINDUSER:
      {
        QString find_user = typePacket;
        qDebug() << commandList;

        if (find_user == "FNDP" && gl_fname!=name)
          {
            QString pubKey = commandList.at(1);
            pubFriendKey[gl_fname] = pubKey;
            QList <QPair<QString, QString> > a;
            AddUserToList(gl_fname, commandList.at(0), a , -1);
            findcont->~findcontacts();
            on_glass_button_clicked();
          }
        else
          findcont->SetErrorLayout(1);
        break;
      }

      // Response for an invitation to friends
    case COMMAND::INVITE:
      {
        QList <QPair<QString, QString> > a;
        QString pubKey = commandList.at(2);
        pubFriendKey[commandList.at(0)] = pubKey;

        AddUserToList(commandList.at(0), commandList.at(1), a , -2);
        break;
      }

      // Information about the user
    case COMMAND::USINFO:
      {
        if (typePacket == "INFP")
          {
            QStringList *usData = new QStringList;
            QStringList UData = message.split(" /s ");
            // 0 - Sex, 1 - Age, 2 - City, 3 - OnlineStatus, 4 - Email/Phone, 5 - LiveStatus

            usData->push_back(vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString());
            usData->push_back(UData.at(0));
            usData->push_back(UData.at(1));
            usData->push_back(UData.at(2));

            users_info = new UsersGroupInfo(this, usData);
            users_info->setGeometry(this->x()+365, this->y()+70, 330, 480);
            SetGlass();
            users_info->show();
            break;
          }
      }

      // Presence status
    case COMMAND::ISONLINE:
      {
        QString username = commandList.at(0);

        if (typePacket == "STON")
          FriendOnlineStatus[username] = "Online";
        else
          {
            FriendOnlineStatus[username] = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
          }

        break;
      }

      // new message
    case COMMAND::NEWMSG:
      {
        QString msg = commandList.at(1);

        // Sound notification
        if (U_ChBox_PSound->isChecked())
          QSound::play(SOUND_FROM);

        if (!vec.empty())
          {
            for (int i = 0; i < vec.size(); i++)
              if (vec.at(i)->data(Qt::DisplayRole) == fromname)
                {
                  QStringList lst = myPrivateKey.split(" ", QString::SkipEmptyParts);
                  QString decodmsg = rsaCrypt->DecodeText(msg, lst.at(0).toInt(), lst.at(1).toInt());

                  QListWidgetItem *item = new QListWidgetItem();
                  item->setData(Qt::UserRole + 1, "FROM");
                  item->setData(Qt::DisplayRole, decodmsg);
                  item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                  vec.at(i)->setData(Qt::UserRole + 1, decodmsg);
                  vec.at(i)->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                  // Visual notification
                  if (!this->isVisible())
                    {
                      notice->SetPopupText(lan_dict.value("notice_msg") + " (" + fromname + "):\n" + decodmsg);
                      notice->Show();
                    }

                  chatvec.at(i)->addItem(item);
                  chatvec.at(i)->scrollToBottom();
                  vec.at(i)->setSelected(true);
                  WhisperOnClickUsers(vec.at(i));
                  U_start_textBrowser->hide();
                  U_stackedWidget_2->show();
                  U_stackedWidget_2->setCurrentIndex(i);

                  break;
                }
            // If the user is not in the list (previously removed) to add to the list again.
            // Make later. Add to the current conversation and again in the database on the server.

            //QList <QPair<QString, QString> > a;
            //AddUser_Chat(fromname, commandList.at(2), a , -1);
            //qDebug() << fromname << message;
          }

        break;
      }

    case COMMAND::GETFILE:
      {
        QByteArray buffer;
        QString filename;
        qint64 fileSize;

        QString dirDownloads = U_label_6->text();
        QDir(dirDownloads).mkdir(dirDownloads);

        //FIXME
        //in >> fromname >> filename >> fileSize;

        QThread::sleep(2);
        forever
        {
          if (!nextBlockSize) {

              if (quint32(tcpSocket->bytesAvailable()) < sizeof(quint32)) {
                  break;
                }
              //FIXME
              //in >> nextBlockSize;
            }
          //FIXME
          //in >> buffer;
          if (tcpSocket->bytesAvailable() < nextBlockSize) {
              break;
            }
        }

        QFile receiveFile(dirDownloads + filename);
        receiveFile.open(QIODevice::ReadWrite);
        receiveFile.write(buffer);
        receiveFile.close();
        buffer.clear();

        QIcon pic(ICON_FILE_PATH);
        if (!vec.empty())
          {
            for (int i=0; i<vec.size(); i++)
              if (vec.at(i)->data(Qt::DisplayRole) == fromname)
                {
                  QListWidgetItem *item = new QListWidgetItem();
                  item->setData(Qt::UserRole + 1, "FROMF");
                  item->setData(Qt::DisplayRole, filename);
                  item->setData(Qt::ToolTipRole, QString::number((float)fileSize/1024, 'f', 2)
                                + " KB  "
                                +  QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                  item->setData(Qt::DecorationRole, pic);
                  vec.at(i)->setData(Qt::UserRole + 1, lan_dict.value("File") + ": " + filename);
                  vec.at(i)->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                  if (!this->isVisible() && U_ChBox_Notif->isChecked())
                    {
                      notice->SetPopupText(lan_dict.value("notice_msg") + " (" + fromname + "):\n" + lan_dict.value("File") + ": " + filename);
                      notice->Show();
                    }

                  chatvec.at(i)->addItem(item);
                  chatvec.at(i)->scrollToBottom();
                  vec.at(i)->setSelected(true);
                  WhisperOnClickUsers(vec.at(i));
                  U_start_textBrowser->hide();
                  U_stackedWidget_2->show();
                  U_stackedWidget_2->setCurrentIndex(i);

                  break;
                }
          }
        nextBlockSize = 0;

        break;
      }

    default:
      {

      }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Cleaning message input when switching chat
void Client::WhisperOnClickUsers(QListWidgetItem* user)
{
  U_editText->clear();
  U_editText->setFocus();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// The movement in the capture window on any area of the window
void Client::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() && Qt::LeftButton) {
      move(event->globalPos() - m_dragPosition);
      event->accept();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// The movement in the capture window on any area of the window
void Client::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
      m_dragPosition = event->globalPos() - frameGeometry().topLeft();
      event->accept();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Information error box
void Client::ShowError(QAbstractSocket::SocketError errorSocket)
{
  switch (errorSocket)
    {
    case QAbstractSocket::RemoteHostClosedError:
      QMessageBox::information(this, tr("W-H-I-S-P-E-R"),
                               lan_dict.value("Disconnected_1"));
      break;
    case QAbstractSocket::HostNotFoundError:
      QMessageBox::information(this, tr("W-H-I-S-P-E-R"),
                               lan_dict.value("ServerNF"));
      break;
    case QAbstractSocket::ConnectionRefusedError:
      QMessageBox::information(this, tr("W-H-I-S-P-E-R"),
                               lan_dict.value("Disconnected_2"));
      break;
    default:
      QMessageBox::information(this, tr("W-H-I-S-P-E-R"),
                               lan_dict.value("Occurred_err") + QString("%1.").arg(tcpSocket->errorString()));
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Sending data when connected to a server
void Client::SendPersonalData()
{
  if (!personDates)
    {
      personDates = true;

      QString request = "LOAD";

      request.append(U_usernameEdit->text());

      tcpSocket->write(request.toUtf8());
      authorization->close();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Disconnect
void Client::OnDisconnect()
{
  personDates = false;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening the settings window.
void Client::on_userSetting_clicked()
{
  U_widget_2->show();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Closing the settings window.
void Client::on_close_setting_button_clicked()
{
  U_userList_3->clearFocus();
  U_userList_3->clearSelection();
  U_stackedWidget->setCurrentIndex(0);
  U_widget_2->hide();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Changing the settings menu.
void Client::WhisperOnClick(QListWidgetItem* User)
{
  QStringList lst_options;
  lst_options = lan_dict.value("userList_3").split(',');

  QString section = User->text();
  if (section == lst_options.at(0))
    U_stackedWidget->setCurrentIndex(0);
  else if (section == lst_options.at(1))
    U_stackedWidget->setCurrentIndex(1);
  else if (section == lst_options.at(2))
    U_stackedWidget->setCurrentIndex(2);
  else if (section == lst_options.at(3))
    U_stackedWidget->setCurrentIndex(3);
  U_userList_3->clearSelection();
  U_setTitle->setText(U_userList_3->currentItem()->text());
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening a list of Emoji
void Client::on_pushButton_clicked()
{
  ShowEmoji();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening a list of Emoji
void Client::ShowEmoji()
{
  QPoint p = QCursor::pos();
  frameEmoji->setGeometry(p.x()-250, p.y() -300, 300, 250);
  frameEmoji->show();

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Smooth dimming the screen.
void Client::SetGlass()
{
  QPropertyAnimation* animation = new QPropertyAnimation(U_glass_button);
  QGraphicsOpacityEffect* grEffect = new QGraphicsOpacityEffect(U_glass_button);
  U_glass_button->setGraphicsEffect(grEffect);

  animation->setTargetObject(grEffect);
  animation->setPropertyName("opacity");
  animation->setDuration(1000);
  animation->setStartValue(0.0);
  animation->setEndValue(1.0);
  animation->start();
  U_glass_button->show();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening the user search window
void Client::ShowUserFound()
{
  QPoint p = QCursor::pos();
  findcont->setGeometry(p.x()-200, p.y()-230, 310, 350);
  findcont->show();

  connect(findcont, SIGNAL(findUsers(QString)), this, SLOT(FindUserInServer(QString)));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Sending a user's search request
void Client::FindUserInServer(QString name_user)
{
  gl_fname = name_user;
  static bool tmp = true;

  if (name == gl_fname)
    {
      findcont->SetErrorLayout(0);
      return;
    }

  for (int i = 0; i < vec.size(); i++)
    if (vec.at(i)->data(Qt::DisplayRole) == gl_fname)
      {
        findcont->SetErrorLayout(2);
        return;
      }

  if (tmp)
    {
      QString request = "FIND";

      request.append(name_user + " /s " + name);

      tcpSocket->write(request.toUtf8());
      tmp=false;
    }
  else    // Костылек :)
    tmp=true;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Selection window, to add a user or group creation
void Client::on_newContact_Button_clicked()
{
  choice_window = new ChoiceCreate(this);
  choice_window->SetLanguage(lan_dict);
  choice_window->setGeometry(this->x()+359, this->y()+130, 343, 220);
  SetGlass();
  choice_window->show();
  connect(choice_window, SIGNAL(choice(QString)), this, SLOT(ChoiceWindow(QString)));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Selection window, to add a user or group creation
void Client::ChoiceWindow(QString str)
{
  if (str == "close")
    on_glass_button_clicked();

  else if (str == "newContact")
    {
      findcont = new findcontacts();
      findcont->SetLanguage(lan_dict);
      ShowUserFound();
    }
  else if (str == "newGroup")
    {
      create_group = new CreateGroup(this);
      create_group->SetLanguage(lan_dict);
      ShowCreateGroup();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Create a group
void Client::ShowCreateGroup()
{
  //QPoint p = QCursor::pos();
  create_group->setGeometry(this->x()+330, this->y()+130, 400, 230);
  create_group->show();

  connect(create_group, SIGNAL(GroupSignal(QString, QString, QString, QString)), SLOT(GetCreateGroupSig(QString, QString, QString, QString)));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Create a group
void Client::GetCreateGroupSig(QString state, QString group_name, QString group_descr, QString path_avatar)
{
  if (state == "Close"){
      on_glass_button_clicked();
    }
  else if (state == "Create")
    {
      selectContacts = new SelectContacts(this, U_userList);
      selectContacts->SetLanguage(lan_dict);
      selectContacts->setGeometry(this->x()+350, this->y()+40, 361, 540);
      selectContacts->show();

      groupData.push_back(group_name);
      groupData.push_back(group_descr);
      groupData.push_back(path_avatar);

      connect(selectContacts, SIGNAL(SelectUsersSignal(QStringList, QString)), SLOT(AddGroupToList(QStringList, QString)));
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Sending a request to create a group. Adding a group to the list
void Client::AddGroupToList(QStringList userList, QString state)
{
  // If click the Close button, the request is not sent, the window closes.
  if (state == "Cancel")
    {
      if (!groupData.isEmpty())
        groupData.clear();
      on_glass_button_clicked();
    }

  else if (state == "Create")
    {
      QListWidgetItem *item = new QListWidgetItem();
      QListWidget *chatlist = new QListWidget();
      chatlist->setItemDelegate(new ChatListDelegate(chatlist, colorchat));
      U_stackedWidget_2->addWidget(chatlist);

      QIcon pic(":/new/prefix1/Resource/group-1.png");
      item->setData(Qt::DisplayRole, groupData.at(0));
      item->setData(Qt::StatusTipRole, "group");
      item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
      item->setData(Qt::UserRole + 1, groupData.at(1));
      item->setData(Qt::DecorationRole, pic);

      QListWidgetItem *item2 = new QListWidgetItem();
      item2->setData(Qt::UserRole + 1, "TO");
      item2->setData(Qt::DisplayRole, lan_dict.value("NewGroup_msg") + " \"" + groupData.at(0) + "\"");
      item2->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
      chatlist->addItem(item2);

      vec.push_back(item);
      chatvec.push_back(chatlist);
      U_userList->addItem(item);
      U_userList->scrollToBottom();

      U_start_textBrowser->hide();
      U_stackedWidget_2->show();
      U_stackedWidget_2->setCurrentIndex(chatvec.size()-1);
      U_userList->clearSelection();
      vec.at(vec.size()-1)->setSelected(true);
      WhisperOnClickUsers(vec.at(vec.size()-1));
      on_glass_button_clicked();

      QByteArray msg;
      QDataStream out(&msg, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_5_4);

      //FIXME: Split userList into Strings .. or send separate sctructure
      out << quint32(0) << QTime::currentTime() << QString("_NEWGROUP_") << groupData.at(0) << groupData.at(1) <<  userList;
      tcpSocket->write(msg);

    }
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///Processor key
void Client::keyReleaseEvent(QKeyEvent *event)
{
  if (U_RB_sendEnter->isChecked())
    {
      switch (event->key()) {
        case Qt::Key_Escape:
          // Processing pressing Esc
          break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
          if (U_widget_2->isHidden() && U_RB_sendEnter->isChecked())
            on_sendMessage_clicked();
          break;
          // Handlers of other keys
        }
    }

  if (U_RB_send_CEnter->isChecked())
    if (event->modifiers() == Qt::ControlModifier)
      if (event->key() == Qt::Key_Return)
        if (U_widget_2->isHidden() && U_RB_send_CEnter->isChecked())
          on_sendMessage_clicked();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Minimize the window. Hide to tray
void Client::on_actionShowHideWindow_triggered()
{
  ShowHideWindow();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Minimize the window. Hide to tray
void Client::on_actionExit_triggered()
{
  QApplication::exit();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Minimize the window. Hide to tray
void Client::IconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
      ShowHideWindow();
      break;
    case QSystemTrayIcon::Unknown:
      break;
    case QSystemTrayIcon::Context:
      break;
    case QSystemTrayIcon::DoubleClick:
      break;
    case QSystemTrayIcon::MiddleClick:
      break;
    default:
      break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Minimize the window. Hide to tray
void Client::ShowHideWindow()
{
  if (this->isVisible()) {
      hide();
      U_actionShowHideWindow->setText(tr("Restore"));
    }
  else {
      show();
      U_actionShowHideWindow->setText(tr("Hide"));
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Overriding the Close button. Hide to tray
void Client::closeEvent(QCloseEvent *event)
{
  ShowHideWindow();
  event->ignore();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening the palette to change the background of the chat
/// Setting single color
void Client::on_PB_SelColor_clicked()
{
  QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
  colorchat = color.name();

  QPixmap pixmap(16,16);
  QPainter painter;
  painter.begin(&pixmap);
  painter.drawRect(0,0,16,16);
  painter.fillRect(0,0,16,16,QBrush(QColor(color)));
  painter.end();
  U_imageLabel->setPixmap(pixmap);
  U_chat_back_lab->setPixmap(pixmap);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Opening the file selection to change chat background.
/// Setting images
void Client::on_PB_LoadFileBackground_clicked()
{
  QString files = QFileDialog::getOpenFileName(this, lan_dict.value("SelectImages"), "" , tr("Images (*.jpg *jpeg *.png)"));

  if (QString::compare(files, QString())!=0)
    {
      QImage image;
      bool vol = image.load(files);

      if (vol)
        {
          U_chat_back_lab->setPixmap(QPixmap::fromImage(image));
          U_imageLabel->setPixmap(QPixmap::fromImage(image));
        }
      else
        {
          return;
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Select and install a new user avatars.
void Client::on_radioButton_2_clicked()
{
  QString files = QFileDialog::getOpenFileName(this, lan_dict.value("SelectImages"), "" , tr("Images (*.jpg *jpeg *.png)"));

  if (QString::compare(files, QString())!=0)
    {
      QImage image;
      bool vol = image.load(files);

      if (vol)
        {
          U_avatar_label->setPixmap(QPixmap::fromImage(image));
          U_avatar_label->setFixedSize(120,120);
        }
      else
        {
          //Error
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Standard for the avatar icons.
/// Change then.
void Client::on_radioButton_clicked()
{
  QImage image(":/Avatars/Resource/Avatars/5.jpg");
  U_avatar_label->setPixmap(QPixmap::fromImage(image));
  U_avatar_label->setFixedSize(120,120);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Selecting the new folder to download files.
void Client::on_Download_path_PB_clicked()
{
  QString path = QFileDialog::getExistingDirectory(this, lan_dict.value("SelectImages"), "");

  if (QString::compare(path, QString())!=0)
    {
      download_path = path;
      U_label_6->setText(path);
    }
  else
    U_label_6->setText(download_path);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Select file for sending to the user from the list.
void Client::on_pushButton_2_clicked()
{
  //FIXME: Sending files
  if (U_stackedWidget_2->isHidden())
    return;

  QString filePatch = QFileDialog::getOpenFileName(this, lan_dict.value("fileWindow"), "", QObject::trUtf8("(*.*)"));

  if (filePatch.isEmpty())
    return;

  QByteArray  arrBlock;
  qint64 fileSize;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_4);

  sendFile = new QFile(filePatch);
  sendFile->open(QFile::ReadOnly);

  QFileInfo fi(filePatch);
  QString fileName = fi.fileName();

  fileSize = fi.size();

  QByteArray buffer = sendFile->readAll();
  QString receiver_name = vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString();

  out << quint32(0) << QTime::currentTime() << QString("_FILE_") << receiver_name
      << fileName << fileSize << buffer;

  out.device()->seek(0);
  out << quint32(arrBlock.size() - sizeof(quint32));

  tcpSocket->write(arrBlock);

  QIcon pic(ICON_FILE_PATH);
  int pos;
  QListWidgetItem *item = new QListWidgetItem();
  item->setData(Qt::UserRole + 1, "TOF");

  if (fileName.size() > 55)
    {
      for (int i = fileName.size()-1; i >= 0; i--)
        if (fileName.at(i) == '.')
          {
            pos = i;
            //fileName.remove(28, pos);
            QString newfilename = fileName.left(50)+"...."+fileName.right(fileName.size()-pos);
            fileName = newfilename;
            break;
          }
    }

  item->setData(Qt::DisplayRole, fileName);
  item->setData(Qt::ToolTipRole, QString::number((float)fileSize/1024, 'f', 2)  + " KB  " + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
  item->setData(Qt::DecorationRole, pic);
  chatvec.at(U_stackedWidget_2->currentIndex())->addItem(item);
  chatvec.at(U_stackedWidget_2->currentIndex())->scrollToBottom();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Select User from the list.
void Client::on_userList_clicked(const QModelIndex &index)
{
  if (!vec.empty())
    {
      U_start_textBrowser->hide();
      U_stackedWidget_2->show();
      U_stackedWidget_2->setCurrentIndex(index.row());
      QString cur_user = vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString();

      qDebug() << cur_user << U_stackedWidget_2->currentIndex();

      QString status = FriendOnlineStatus.value(cur_user);

      // Setting in the header name of the selected friend
      U_head_name->setText(cur_user);

      QDate _Today = QDate::currentDate();
      QDate _Date = QDate::fromString(status.left(10), "dd.MM.yyyy");
      qint32 difference_days = _Today.daysTo(_Date);

      if (status == "Online")
        U_online_status->setText(status);

      else {
          switch (difference_days)
            {
            case -1:
              U_online_status->setText(lan_dict.value("was_yesterday") + status.right(status.size()-10));
              break;
            case 0:
              U_online_status->setText(lan_dict.value("was_today") + status.right(status.size()-10));
              break;
            default:
              {
                QString setHead = Pronoun(cur_user)
                    + lan_dict.value("was_online")
                    +  QString::number(abs(difference_days))
                    + " "
                    + SuffixDay(difference_days)
                    + lan_dict.value("ago");

                U_online_status->setText(setHead);
                break;
              }
            }
        }
    }
}

QString Client::Pronoun(QString username)
{
  QString pronoun;
  QStringList sex_person = lan_dict.value("sex_person").split(',');

  if (SexOfUsers[username] == sex_person.at(2))
    pronoun = lan_dict.value("she_was");
  else pronoun = lan_dict.value("he_was");;

  return pronoun;
}

QString Client::SuffixDay(int difference_days)
{
  QString suffix_day;
  int day = abs(difference_days) % 100;

  if (day > 10 && day < 20)
    suffix_day = lan_dict.value("days");
  else
    {
      day = day % 10;
      if (day == 1)
        suffix_day = lan_dict.value("day");
      else if (day>=2 && day <= 4)
        suffix_day = lan_dict.value("of_the_day");
      else
        suffix_day = lan_dict.value("days");
    }

  return suffix_day;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Close - hide in the system tray
void Client::on_pushButton_3_clicked()
{
  this->close();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Dialogue Program
void Client::on_pushButton_5_clicked()
{
  aboutdialog = new AboutDialog(this);
  aboutdialog->setGeometry(this->x()+340, this->y()+70, 380, 480);
  SetGlass();
  aboutdialog->show();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Clear history
void Client::ClearHistory()
{
  // While the session, add a confirmation and a request to the database to clean up correspondence.

  conf_message = new ConfirmWindow(this, lan_dict.value("conf_message_del_hist"));
  conf_message->SetLanguage(lan_dict);
  conf_message->setGeometry(this->x()+355, this->y()+100, 350, 170);
  SetGlass();
  conf_message->show();

  connect(conf_message, SIGNAL(response(QString)), this, SLOT(ClearCurHistory(QString)));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Sending a request to the server to clean up the history of the user
void Client::ClearCurHistory(QString cmd)
{
  conf_message->hide();
  on_glass_button_clicked();

  if (cmd == "OK")
    {
      chatvec.at(U_stackedWidget_2->currentIndex())->clear();

      QString request = "CLNH";

      request.append(name + " /s " + vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString());

      tcpSocket->write(request.toUtf8());
    }

  //conf_message->~ConfirmWindow();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Removing a user from the list
void Client::ClearCurUser(QString cmd)
{
  conf_message->hide();

  if (cmd == "OK")
    {
      // If you have contacts in the list, remove the right.
      if (!U_userList->size().isEmpty())
        {
          QString request = "DELF";

          request.append(name + " /s " + vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString());

          tcpSocket->write(request.toUtf8());

          // Removing a widget from the Widget Stack
          QWidget* widget = U_stackedWidget_2->widget(U_userList->currentRow());
          // and remove
          U_stackedWidget_2->removeWidget(widget);

          // Clean vector
          vec.erase(vec.begin()+U_userList->currentRow());
          chatvec.erase(chatvec.begin()+U_userList->currentRow());
          U_userList->removeItemWidget(U_userList->takeItem(U_userList->currentRow()));
          update();
        }
    }

  on_glass_button_clicked();
  //conf_message->~ConfirmWindow();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Take off selection from posts
void Client::ClearSelect()
{
  chatvec.at(U_stackedWidget_2->currentIndex())->clearSelection();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Context menu for a chat
void Client::ShowContextMenuForChat(const QPoint &pos)
{
  if (U_stackedWidget_2->count()==0 || U_stackedWidget->isHidden() || chatvec.at(U_stackedWidget_2->currentIndex())->count()==0)
    return;

  QPoint newPos = pos;
  newPos.setX(pos.x()+385);
  newPos.setY(pos.y()+30);

  QMenu * menu = new QMenu(this);
  QAction * deleteDevice = new QAction(lan_dict.value("ContextClearHistory"), this);
  QAction * delSelect = NULL;
  menu->addAction(deleteDevice);

  // If there is a selected messages, add the menu skipnut opportunity.
  if (U_stackedWidget_2->count()!=0)
    if (!chatvec.at(U_stackedWidget_2->currentIndex())->selectedItems().isEmpty())
      {
        delSelect = new QAction(lan_dict.value("ContextClearSelection"), this);
        menu->addAction(delSelect);
      }

  menu->popup(mapToGlobal(newPos));
  connect(delSelect, SIGNAL(triggered()), this, SLOT(ClearSelect())); // Обработчик удаления записи
  connect(deleteDevice, SIGNAL(triggered()), this, SLOT(ClearHistory()));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Context menu for a contact list
void Client::ShowContextMenuForWidget(const QPoint &pos)
{
  if (U_userList->count() ==0 || U_userList->selectedItems().isEmpty())
    return;

  QPoint newPos = pos;
  newPos.setX(pos.x());
  newPos.setY(pos.y()+45);

  QMenu * menu = new QMenu(this);
  QAction * deleteDevice = new QAction(lan_dict.value("ContextDelUser"), this);
  connect(deleteDevice, SIGNAL(triggered()), this, SLOT(DeleteUser())); // Обработчик удаления записи
  menu->addAction(deleteDevice);
  menu->popup(mapToGlobal(newPos));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Loading standard language when the application starts.
void Client::SetDefaultLanguage()
{
  if (!lan_dict.isEmpty())
    lan_dict.clear();

  lan_dict = set_language->ParseXML(DEFAULT_LANGUAGE);
  SetLanguage();
  authorization->SetLanguage(lan_dict);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Setting the standard language when the application starts.
void Client::SetLanguage()
{
  U_language->setText(lan_dict.value(U_language->objectName()));
  U_down_path->setText(lan_dict.value(U_down_path->objectName()));
  U_RB_sendEnter->setText(lan_dict.value(U_RB_sendEnter->objectName()));
  U_RB_send_CEnter->setText(lan_dict.value(U_RB_send_CEnter->objectName()));
  U_ChBox_Notif->setText(lan_dict.value(U_ChBox_Notif->objectName()));
  U_ChBox_PSound->setText(lan_dict.value(U_ChBox_PSound->objectName()));
  U_background->setText(lan_dict.value(U_background->objectName()));
  U_PB_SelColor->setText(lan_dict.value(U_PB_SelColor->objectName()));
  U_PB_LoadFileBackground->setText(lan_dict.value(U_PB_LoadFileBackground->objectName()));
  U_username_label->setText(lan_dict.value(U_username_label->objectName()));
  U_radioButton->setText(lan_dict.value(U_radioButton->objectName()));
  U_radioButton_2->setText(lan_dict.value(U_radioButton_2->objectName()));
  U_groupBox->setTitle(lan_dict.value(U_groupBox->objectName()));
  U_search_line_edit->setPlaceholderText(lan_dict.value(U_search_line_edit->objectName()));
  U_editText->setPlaceholderText(lan_dict.value(U_editText->objectName()));
  U_start_sys->setText(lan_dict.value(U_start_sys->objectName()));
  //U_start_textBrowser->setText(lan_dict.value(U_start_textBrowser->objectName()));

  QStringList lst_setting;
  lst_setting = lan_dict.value("userList_3").split(',');

  for (int i = 0; i < lst_setting.size(); i++)
    U_userList_3->item(i)->setText(lst_setting[i]);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Remove views selections
void Client::RemoveSelections()
{
  U_search_line_edit->setAttribute(Qt::WA_MacShowFocusRect, false);
  U_editText->setAttribute(Qt::WA_MacShowFocusRect, false);
  U_userList_3->setAttribute(Qt::WA_MacShowFocusRect, false);
  U_userList->setAttribute(Qt::WA_MacShowFocusRect, false);
  U_stackedWidget->setAttribute(Qt::WA_MacShowFocusRect, false);

  // ???
  U_stackedWidget_2->setAttribute(Qt::WA_MacShowFocusRect, false);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Removing a user
void Client::DeleteUser()
{
  // And the request to the server to the database, remove from the table.
  // Don't know that it was time to do on the side of the remote, then he would write, and he's gone.

  conf_message = new ConfirmWindow(this, lan_dict.value("conf_message_del_user"));
  conf_message->SetLanguage(lan_dict);
  conf_message->setGeometry(this->x()+355, this->y()+100, 350, 170);
  SetGlass();
  conf_message->show();

  connect(conf_message, SIGNAL(response(QString)), this, SLOT(ClearCurUser(QString)));
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Search for the contact - list. Called when text is changed. field.
void Client::on_search_line_edit_textChanged(const QString &arg1)
{
  // If the query is empty, delete the previous search results + hide the widget.
  if (arg1.isEmpty()){
      U_search_list->clear();
      U_search_list->hide();
      return;
    }

  //If the contacts list is empty, I find occurrences of the search query in the names of friends.
  if (!vec.isEmpty())
    {
      U_search_list->show();
      U_search_list->clear();
      for (int i = 0; i < vec.size(); i++)
        if (vec.at(i)->data(Qt::DisplayRole).toString().contains(arg1, Qt::CaseInsensitive))
          {
            // To add a temporary list of items that match the search (occurrence of a string)
            // Rewrite not copy all of the items and take them to the link.
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::DisplayRole, vec.at(i)->data(Qt::DisplayRole).toString());
            item->setData(Qt::ToolTipRole, vec.at(i)->data(Qt::ToolTipRole).toString());
            item->setData(Qt::UserRole + 1, vec.at(i)->data(Qt::UserRole + 1).toString());
            item->setData(Qt::DecorationRole, vec.at(i)->data(Qt::DecorationRole));
            U_search_list->addItem(item);
          }
    }
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Selecting a contact from the search query (list)
void Client::on_search_list_clicked(const QModelIndex &index)
{
  for (int i = 0; i < vec.size(); i++)
    {
      if (vec.at(i)->data(Qt::DisplayRole).toString() == index.data(Qt::DisplayRole).toString())
        {
          // Switching to a selected person / chat. Cleansing search request + Hiding result.
          U_start_textBrowser->hide();
          U_stackedWidget_2->show();
          U_stackedWidget_2->setCurrentIndex(i);
          U_userList->setCurrentRow(i);
          U_search_line_edit->clear();
          U_search_line_edit->clearFocus();
          U_search_list->hide();
          U_search_list->clear();
          return;
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Hide darkened window
void Client::on_glass_button_clicked()
{
  //delete choice_window;
  U_glass_button->hide();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Request for information about the user
void Client::on_info_user_button_clicked()
{
  if (U_stackedWidget_2->count()==0 || U_stackedWidget->isHidden())
    return;

  QString request = "GETI";

  request.append(vec.at(U_userList->currentRow())->data(Qt::DisplayRole).toString());

  tcpSocket->write(request.toUtf8());
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Switching and installation of a new language
void Client::on_select_language_box_currentIndexChanged(int index)
{
  lan_dict.clear();

  switch (index) {
    case 0:
      {
        lan_dict = set_language->ParseXML(DEFAULT_LANGUAGE);
        break;
      }
    case 1:
      {
        lan_dict = set_language->ParseXML("FR_Language");
        break;
      }
    case 2:
      {
        lan_dict = set_language->ParseXML("DE_Language");
        break;
      }
    case 3:
      {
        lan_dict = set_language->ParseXML("RU_Language");
        break;
      }
    }

  SetLanguage();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Dumb destructor. Append.
Client::~Client()
{
  delete ui;
}

