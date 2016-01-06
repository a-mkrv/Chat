#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QFileDialog>
#include <QString>
#include <QPainter>


Client::Client(QWidget *parent) : QMainWindow(parent), ui(new Ui::Client)
{
    ui->setupUi(this);
    frameEmoji = new EmojiFrame();
    emojiMan = new EmojiManager();

    reg_window = new registration();
    connect(reg_window, SIGNAL(sendData(QString, QString)), this, SLOT(recieveData(QString, QString)));
    connect(reg_window, SIGNAL(sendFindContact(QString)), this, SLOT(recieveUser(QString)));

    ui->RB_sendEnter->setChecked(true);
    download_path="(C:/...)";
    stackchat = new QStackedWidget;

    trayIcon = new TrayIcon(this);
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->actionShowHideWindow);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionExit);
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->hide();

    ui->chatDialog->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
    ui->chatDialog->clearSelection();


    QPixmap back_to_menu(":/new/prefix1/Resource/double78.png");
    QIcon ButtonIcon(back_to_menu);

    QPixmap setting(":/new/prefix1/Resource/tool257.png");
    QIcon ButtonIcon2(setting);

    QPixmap add_contact(":/new/prefix1/Resource/square292.png");
    QIcon ButtonIcon3(add_contact);

    QPixmap send(":/new/prefix1/Resource/right-arrow6.png");
    QIcon ButtonIcon4(send);

    QPixmap search_mes(":/new/prefix1/Resource/magnifying glass57.png");

    //QImage image(":/new/prefix1/Resource/bumaga.jpg");
    ui->chat_back_lab->setStyleSheet("background-color: rgb(255, 255, 235)");
    ui->chatDialog->setStyleSheet(""" color: white; background-image: url(:/new/prefix1/Resource/bg3.jpg);background-attachment: scroll;""");

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

    connect(frameEmoji, SIGNAL(sendEmoji(QString)), this, SLOT(insertEmoticon(QString)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(show_Error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(send_personal_data()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(ui->userSetting_button, SIGNAL(clicked()), this, SLOT(on_userSetting_clicked()));
    connect(ui->close_setting_button_2, SIGNAL(clicked()), this, SLOT(on_close_setting_button_clicked()));
    connect(ui->userList_3, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(whisperOnClick(QListWidgetItem*)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    ui->chatDialog->setStyleSheet("background: transparent;");

}

 void Client::keyReleaseEvent(QKeyEvent *event)
{
     if(ui->RB_sendEnter->isChecked())
     {
    switch(event->key()) {
    case Qt::Key_Escape:
       // Обработка нажатия Esc
       break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if(ui->widget_2->isHidden() && ui->RB_sendEnter->isChecked())
       on_sendMessage_clicked();
       break;
     // Обработчики других клавиш
    }
    }

     if(ui->RB_send_CEnter->isChecked())
     {
        if(event->modifiers()==Qt::ControlModifier)
        {
            if(event->key() == Qt::Key_Return)
            {
                if(ui->widget_2->isHidden() && ui->RB_send_CEnter->isChecked())
                    on_sendMessage_clicked();
       }
     }
   }
}

void Client::recieveData(QString str, QString pas)
{
   if(str!="" && pas!="")
   {
        name = str;
        name.replace(" ", "_");
        ui->usernameEdit->setText(name);
        this->show();
        trayIcon->show();
   }
}

void Client::recieveUser(QString str)
{
    //if(str!="")
    //{
    //    ui->editText->setText("123");
    //}
}

void Client::on_sendMessage_clicked()
{
    QRegExp rx("<a [^>]*name=\"([^\"]+)\"[^>]*>");
    QString str = ui->editText->text();
    QString message = str;
    QStringList list;

    list.clear();
    int pos = 0;

    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }

    for (int i = 0; i < list.count(); i++) {
        QString emojiNumber = list.at(i);

        QString searchTag = QString("<a name=\"%1\"></a>")
                .arg(emojiNumber);

        QString replTag = emojiMan->getEmojiSymbolFromNumber(emojiNumber);

        str = str.replace(searchTag, replTag);
    }

    // QTextDocument doc;
    //doc.setHtml(str);

    //return doc.toPlainText();
    ui->editText->clear();

    blockSize = 0;

    if (!message.isEmpty())              //Отправка сообщений
    {
        if (message.at(0) == '/')        //Личное сообщение / команда
        {
            sendUserCommand(message);
        }
        else                            //Сообщение на сервер
        {
            QByteArray msg;
            QDataStream out(&msg, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_4);
            new QListWidgetItem(name + ": " +  message, ui->chatDialog);
            QSound::play(":/new/prefix1/Resource/to.wav");

            out << message;
            tcpSocket->write(msg);
        }
    }
}

void Client::insertEmoticon(QString symbol)
{
    QTextCursor cursor(ui->textEdit->textCursor());

    if (!cursor.isNull()) {
        cursor.insertHtml(symbol);
    }

    QTextCursor cursor2(ui->textEdit->document()->find(symbol));

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

void Client::on_connect_button_clicked()
{
    QString hostname = "127.0.0.1";
    quint16 port = 55155;
    QString status = tr("-> Connecting to 127.0.0.1 on port 55155.");

    new QListWidgetItem(status, ui->chatDialog);
    tcpSocket->abort();
    tcpSocket->connectToHost(hostname, port);
}

void Client::getMessage()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);

    QString message;
    in >> message;


    enum class COMMAND { NONE, USERLIST, FINDUSER};
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

        for (auto i : commandList)
        {
            q = new QListWidgetItem(i, ui->userList);
            q->setSizeHint(QSize(0,65));
            q->setIcon(pic);
        }
        break;
    }
    case COMMAND::FINDUSER:
    {
        QString find_user;

        break;
    }
    default:
       QSound::play(":/new/prefix1/Resource/from.wav");
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
      // ui->chatDialog->scrollToBottom();

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        QString command = "_USR_";
        QString username = ui->usernameEdit->text();
        out << command;
        out << username;
        tcpSocket->write(block);
        reg_window->hide();
    }
}

void Client::onDisconnect()
{
    new QListWidgetItem("Disconnected..", ui->chatDialog);
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
    else if(section=="General")
        ui->stackedWidget->setCurrentIndex(1);
    else if (section == "Chat options")
        ui->stackedWidget->setCurrentIndex(2);
    else
        ui->stackedWidget->setCurrentIndex(3);
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
    connect(findcont, SIGNAL(findUsers(QString)), this, SLOT(findtoserv(QString)));
}


void Client::findtoserv(QString name_user)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << name_user;
    tcpSocket->write(msg);
}

void Client::on_newContact_Button_clicked()
{
    findcont = new findcontacts();
    showFindCont();
}

void Client::on_actionShowHideWindow_triggered()
{
     showHideWindow();
}

void Client::on_actionExit_triggered()
{
    QApplication::exit();
}

void Client::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        showHideWindow();
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

void Client::showHideWindow()
{
    if (this->isVisible()) {
        hide();
        ui->actionShowHideWindow->setText(tr("Restore"));
    }
    else {
        show();
        ui->actionShowHideWindow->setText(tr("Hide"));
    }
}

void Client::closeEvent(QCloseEvent *event)
{
    showHideWindow();
    event->ignore();
}


Client::~Client()
{
    delete ui;
}

void Client::on_PB_SelColor_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString textColorName = color.name();

    QPixmap pixmap(16,16);
        QPainter painter;
        painter.begin(&pixmap);
        painter.drawRect(0,0,16,16);
        painter.fillRect(0,0,16,16,QBrush(QColor(color)));
        painter.end();
        ui->imageLabel->setPixmap(pixmap);
        ui->chat_back_lab->setPixmap(pixmap);

        // ui->imageLabel->setStyleSheet("background-color: " + textColorName);
        // ui->chatDialog->setStyleSheet("background-color: " + textColorName);

}

void Client::on_PB_LoadFileBackground_clicked()
{
    QString files = QFileDialog::getOpenFileName
            (
                this,
                tr("Select Images"),"",
                tr("Images (*.jpg *jpeg *.png)")
             );

    if(QString::compare(files, QString())!=0)
    {
        QImage image;
        bool vol = image.load(files);

        if(vol)
        {
            ui->chat_back_lab->setPixmap(QPixmap::fromImage(image));
            ui->imageLabel->setPixmap(QPixmap::fromImage(image));

            // Стиль для QListWidget, установка фона.
            //ui->chatDialog->setStyleSheet(
            //            "background-image: url(" + files  + "); "
            //            "max-width:677px;"
            //            "max-height:421px; "
            //             "background-position: center;");

        }
        else
        {
            //Error
        }
    }
}

void Client::on_radioButton_2_clicked()
{
    QString files = QFileDialog::getOpenFileName
            (
                this,
                tr("Select Images"),"",
                tr("Images (*.jpg *jpeg *.png)")
             );

    if(QString::compare(files, QString())!=0)
    {
        QImage image;
        bool vol = image.load(files);

        if(vol)
        {
            ui->avatar_label->setPixmap(QPixmap::fromImage(image));
            ui->avatar_label->setFixedSize(120,120);
        }
        else
        {
            //Error
        }
    }
}

void Client::on_radioButton_clicked()
{
    QImage image(":/new/prefix1/Resource/users53.png");
    ui->avatar_label->setPixmap(QPixmap::fromImage(image));
    ui->avatar_label->setFixedSize(120,120);
}

void Client::on_Download_path_PB_clicked()
{
    QString path = QFileDialog::getExistingDirectory
            (
                this,
                tr("Select Images"),""
             );
    if(QString::compare(path, QString())!=0)
    {
        download_path = path;
        ui->label_6->setText(path);
    }
    else
        ui->label_6->setText(download_path);
}


void Client::on_pushButton_2_clicked()
{
    QByteArray block;
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_4);
    QFile file("C:\\test.jpg");
    file.open(QIODevice::ReadOnly);
    QByteArray buf = file.readAll();
    stream << quint64(file.size());
    stream << buf;
    tcpSocket->write(block);
    tcpSocket->flush();
}
