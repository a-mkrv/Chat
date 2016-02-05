#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QFileDialog>
#include <QString>
#include <QPainter>
#include <time.h>
#include <QPropertyAnimation>

QString gl_fname; //Поиск человека


// Завтра доделать разделение на приватные беседы.  -- Сделано
// Стэк Списков, вектора на адреса списков.         -- Сделано
// Попробовать добавить 1-2 языка..                 -- Сделано (Русский)
// Изменил верхнюю панель, завтра прикручу)         -- Сделано
// Доделать обновление статуса(последнего сообщения) под ником у друзей                         --Сделано
// При добавлении в список друзей просматривать Пол, и в зависимости от этого ставить аватар.   -- Сделано
// Отправку сообщения по текущему диалогу, а не по команде msg User     --Сделано
// Добавлены уведомления в углу экрана.
// Дисконнект из-за доп.сокетов и соединений при авторизации. (Правильно прикрутить закрытие сокета) -- Сделано
// СОХРАНЕНИЕ ПОЛЬЗОВАТЕЛЕЙ Друг у Друга (Через SQLite) --Сделано


// Основное:
// Устойчивая отправка файлов до клииента           -- Временно не работате =(


// Второй план:
// Разбить код по доп классам, функциям (УБРАТЬ НАСТРОЙКИ В ДРУГОЕ МЕСТО!!!!!))
// Возможно исправить делегата, сделав сообщения пузырьком.
// Т.к есть структура со всей инфой о человеке(город, пол и т.д), мб добавлю Информационное окно, где-то нужно инфу разместить в общем.



Client::Client(QWidget *parent) : QMainWindow(parent), download_path("(C:/...)"), personDates(false), ui(new Ui::Client)
{
    srand((time(NULL)));
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    frameEmoji      = new EmojiFrame();
    emojiMan        = new EmojiManager();
    notice          = new Notification();
    reg_window      = new registration();
    trayIcon        = new TrayIcon(this);
    stackchat       = new QStackedWidget;
    trayIconMenu    = new QMenu(this);
    tcpSocket       = new QTcpSocket(this);


    ui->RB_sendEnter->setChecked(true);
    ui->userList->setItemDelegate(new ListDelegate(ui->userList));
    ui->widget_2->hide();

    trayIconMenu->addAction(ui->actionShowHideWindow);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionExit);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->hide();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(reg_window, SIGNAL(sendData(QString, QString)), this, SLOT(recieveData(QString, QString)));
    connect(frameEmoji, SIGNAL(sendEmoji(QString)), this, SLOT(insertEmoticon(QString)));

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(show_Error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(send_personal_data()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

    connect(ui->userSetting_button, SIGNAL(clicked()), this, SLOT(on_userSetting_clicked()));
    connect(ui->close_setting_button_2, SIGNAL(clicked()), this, SLOT(on_close_setting_button_clicked()));
    connect(ui->userList_3, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(whisperOnClick(QListWidgetItem*)));
    connect(ui->userList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(whisperOnClickUsers(QListWidgetItem*)));
    connect(ui->userList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(whisperOnClickSelectUsers(QListWidgetItem*)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
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
        if(event->modifiers()==Qt::ControlModifier)
            if(event->key() == Qt::Key_Return)
                if(ui->widget_2->isHidden() && ui->RB_send_CEnter->isChecked())
                    on_sendMessage_clicked();
}

void Client::recieveData(QString str, QString pas)
{
    if(str!="" && pas!="")
    {
        name = str;
        name.replace(" ", "_");
        ui->usernameEdit->setText(name);

        QString hostname = "127.0.0.1";
        quint16 port = 55155;
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

void Client::on_sendMessage_clicked()
{
    QRegExp rx("<a [^>]*name=\"([^\"]+)\"[^>]*>");      // Регулярные выражения для парсинга смайликов в сообщении
    QString str = ui->editText->text();                 // Получение сообщения для отправки
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
        QString searchTag = QString("<a name=\"%1\"></a>").arg(emojiNumber);
        QString replTag = emojiMan->getEmojiSymbolFromNumber(emojiNumber);
        str = str.replace(searchTag, replTag);
    }

    QTextDocument doc;
    doc.setHtml(ui->textEdit1->toPlainText());
    doc.toPlainText();

    ui->editText->clear();
    blockSize = 0;

    if (!message.isEmpty() && vec.size()!=0)              //Отправка сообщений
    {
        if(ui->ChBox_PSound->isChecked())
            QSound::play(":/new/prefix1/Resource/to.wav");

        QString new_mes = "/msg " + vec.at(ui->userList->currentRow())->data(Qt::DisplayRole).toString() + " " + message;
        sendUserCommand(new_mes);
    }
}

void Client::insertEmoticon(QString symbol)
{
    QTextCursor cursor(ui->textEdit1->textCursor());

    if (!cursor.isNull()) {
        cursor.insertHtml(symbol);
    }

    QTextCursor cursor2(ui->textEdit1->document()->find(symbol));

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

void Client::AddUser_Chat(QString _username, QString _sex, QString _message)
{
    int rand_avatar;
    QString sex = _sex;

    if(sex=="Man")
        rand_avatar = rand()%18+1;
    else if(sex=="Woman")
        rand_avatar = rand()%13+19;
    else if(sex=="Unknown")
        rand_avatar = rand()%20+32;

    QIcon pic(":/Avatars/Resource/Avatars/"+QString::number(rand_avatar)+".jpg");

    if (!vec.empty())
        for(int i=0; i<vec.size(); i++)
            if(vec.at(i)->data(Qt::DisplayRole)==_username)
                return;

    QListWidgetItem *item = new QListWidgetItem();
    QListWidget *chatlist = new QListWidget();
    chatlist->setItemDelegate(new ChatListDelegate(chatlist));
    ui->stackedWidget_2->addWidget(chatlist);

    item->setData(Qt::DisplayRole, _username);
    item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
    item->setData(Qt::UserRole + 1, _message);
    item->setData(Qt::DecorationRole, pic);

    vec.push_back(item);
    chatvec.push_back(chatlist);
    ui->userList->addItem(item);
}

void Client::getMessage()
{
    QString message;
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);
    in >> message;

    QStringList commandList;
    QString fromname;

    enum class COMMAND { NONE, USERLIST, FINDUSER, INVITE};
    COMMAND cmd = COMMAND::NONE;

    if (message=="FRLST")
        cmd = COMMAND::USERLIST;
    else
    {
        commandList = message.split(" ", QString::SkipEmptyParts);
        if(commandList.at(1)==name)
            return;
        fromname = commandList.at(1);

        QStringRef checkCmd(&message, 0, 5);        //Создание строки из заданного диапозона пришедшего сообщения
        if (checkCmd == "_FIN_")
            cmd = COMMAND::FINDUSER;
        if (checkCmd == "_INV_")
            cmd = COMMAND::INVITE;
    }

    switch (cmd)
    {

    case COMMAND::USERLIST:
    {
        QVector <QPair<QString, QString>> lst;
        in >> lst;

        for(int i=0; i<lst.size(); i++)
            AddUser_Chat(lst.at(i).first, lst.at(i).second, "New User.");

        break;
    }

    case COMMAND::FINDUSER:
    {
        QString find_user = commandList.at(1);

        if(find_user=="OKFIN" && gl_fname!=name)
        {
            AddUser_Chat(gl_fname, commandList.at(2), "New User.");
            findcont->~findcontacts();
        }
        else
            findcont->SetErrorLayout(true);
        break;
    }

    case COMMAND::INVITE:
    {
        // Пока без предложения дружбы.
        // Не знаю, нужно ли автоматически добавлять в друзья на той стороне.

        QString find_user = commandList.at(1);
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::DisplayRole, gl_fname);
        item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
        item->setData(Qt::UserRole + 1, "New User.");
        //item->setData(Qt::DecorationRole, pic);
        vec.push_back(item);
        ui->userList->addItem(item);
        break;
    }

    default:                                                // Получение обычного текстового сообщения. Звук и добавление в ЧатЛист
        if(ui->ChBox_PSound->isChecked())
            QSound::play(":/new/prefix1/Resource/from.wav");

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::DisplayRole, message);
        item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

        if(QStringRef(&message, 0, 3)=="*To")
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole + 1, "TO");
            item->setData(Qt::DisplayRole, message.remove(0, 6+fromname.size()));
            item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
            chatvec.at(ui->stackedWidget_2->currentIndex())->addItem(item);
            //item->setData(Qt::UserRole + 1, "TO");
        }
        else
        {
            fromname.chop(1);
            qDebug()  << fromname;
            if (!vec.empty())
            {
                for(int i=0; i<vec.size(); i++)
                    if(vec.at(i)->data(Qt::DisplayRole)==fromname)
                    {
                        QListWidgetItem *item = new QListWidgetItem();
                        item->setData(Qt::UserRole + 1, "FROM");
                        item->setData(Qt::DisplayRole, message.remove(0, 9+fromname.size()));
                        item->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                        vec.at(i)->setData(Qt::UserRole + 1, message);
                        vec.at(i)->setData(Qt::ToolTipRole, QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));

                        if(!this->isVisible())
                        {
                            notice->setPopupText("New message (" + fromname + "):\n" + message);
                            notice->show();
                        }
                        chatvec.at(i)->addItem(item);
                        vec.at(i)->setSelected(true);
                        ui->stackedWidget_2->setCurrentIndex(i);

                        break;
                    }
            }
        }
    }
}


void Client::whisperOnClickUsers(QListWidgetItem* user)
{
    ui->editText->clear();
    QString insert = "/msg " + user->text() + " ";
    ui->editText->setText(insert);
    ui->editText->setFocus();
}

void Client::whisperOnClickSelectUsers(QListWidgetItem* user)
{

}

void Client::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void Client::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
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

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        QString command = "_USR_";
        QString username = ui->usernameEdit->text();
        out << quint16(0) << QTime::currentTime() << command;
        out << username;
        tcpSocket->write(block);
        reg_window->close();
    }
}

void Client::onDisconnect()
{
    ui->userList->clear();
    personDates = false;
}

void Client::sendUserCommand(QString command)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << quint16(0) << QTime::currentTime() << QString("_UCD_") << command;
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
    if (section == "Profile" || section == "Профиль")
        ui->stackedWidget->setCurrentIndex(0);
    else if(section=="General" || section == "Основное")
        ui->stackedWidget->setCurrentIndex(1);
    else if (section == "Chat options" || section == "Опции чата")
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
    findcont->setGeometry(p.x() +380, p.y() +70, 310, 350);
    findcont->show();
    connect(findcont, SIGNAL(findUsers(QString)), this, SLOT(findtoserv(QString)));
}

void Client::findtoserv(QString name_user)
{
    gl_fname=name_user;
    static bool tmp = true;

    if(name==gl_fname)
    {
        findcont->SetErrorLayout(false);
        return;
    }

    if(tmp)
    {
        QByteArray msg;
        QDataStream out(&msg, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        out << quint16(0) << QTime::currentTime() << QString("_FND_") << name_user << name;
        tcpSocket->write(msg);
        tmp=false;
    }
    else    // Костылек :)
        tmp=true;
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
}

void Client::on_PB_LoadFileBackground_clicked()
{
    QString files = QFileDialog::getOpenFileName(this, tr("Select Images"), "" , tr("Images (*.jpg *jpeg *.png)"));

    if(QString::compare(files, QString())!=0)
    {
        QImage image;
        bool vol = image.load(files);

        if(vol)
        {
            ui->chat_back_lab->setPixmap(QPixmap::fromImage(image));
            ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        }
        else
        {
            //Error
        }
    }
}

void Client::on_radioButton_2_clicked()
{
    QString files = QFileDialog::getOpenFileName(this, tr("Select Images"), "" , tr("Images (*.jpg *jpeg *.png)"));

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
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Images"), "");

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
    QString filePatch = QFileDialog::getOpenFileName(this,
                                                     QObject::trUtf8("Выбор файла для отправки"), "",
                                                     QObject::trUtf8("(*.*)"));
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

    out << quint16(0) << QTime::currentTime() << QString("_FILE_")
        << fileName << fileSize << buffer;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug() << arrBlock.size();
    tcpSocket->write(arrBlock);
}

void Client::on_userList_clicked(const QModelIndex &index)
{
    if (!vec.empty())
        ui->stackedWidget_2->setCurrentIndex(index.row());
}

void Client::on_pushButton_3_clicked()
{
    this->close();
}

void Client::on_pushButton_5_clicked()
{
    aboutdialog = new AboutDialog();
    QPoint p = QCursor::pos();
    aboutdialog->setGeometry(p.x() -680, p.y() +80, 380, 480);
    aboutdialog->show();
}

void Client::on_comboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
    {
        ui->checkBox_3->setText("Загрузка при старте системы");
        ui->label_2->setText("Язык: ");
        ui->label_3->setText("Загрузочный путь: ");
        ui->RB_sendEnter->setText("Отправка по \"Enter\"");
        ui->RB_send_CEnter->setText("Отправка по \"Ctrl + Enter\"");
        ui->ChBox_Notif->setText("Уведомления");
        ui->ChBox_PSound->setText("Звук");
        ui->label_7->setText("Выбор фона");
        ui->PB_SelColor->setText("Выбрать из палитры:");
        ui->PB_LoadFileBackground->setText("Загрузить из файла:");
        ui->username_label->setText("Имя пользователя: ");
        ui->radioButton->setText("По умолчанию");
        ui->radioButton_2->setText("Загрузить");
        ui->groupBox->setTitle("Аватар");
        ui->userList_3->item(0)->setText("Профиль");
        ui->userList_3->item(1)->setText("Основное");
        ui->userList_3->item(2)->setText("Опции чата");
        ui->userList_3->item(3)->setText("Сеть");
        ui->search_line_edit->setPlaceholderText("Поиск");
        ui->editText->setPlaceholderText("Введите сообщение..");
        break;
    }
    case 1:
    {
        ui->checkBox_3->setText("Загрузка при старте системы");
        ui->label_2->setText("Язык: ");
        ui->label_3->setText("Загрузочный путь: ");
        ui->RB_sendEnter->setText("Отправка по \"Enter\"");
        ui->RB_send_CEnter->setText("Отправка по \"Ctrl + Enter\"");
        ui->ChBox_Notif->setText("Уведомления");
        ui->ChBox_PSound->setText("Звук");
        ui->label_7->setText("Выбор фона");
        ui->PB_SelColor->setText("Выбрать из палитры:");
        ui->PB_LoadFileBackground->setText("Загрузить из файла:");
        ui->username_label->setText("Имя пользователя: ");
        ui->radioButton->setText("По умолчанию");
        ui->radioButton_2->setText("Загрузить");
        ui->groupBox->setTitle("Аватар");
        ui->userList_3->item(0)->setText("Профиль");
        ui->userList_3->item(1)->setText("Основное");
        ui->userList_3->item(2)->setText("Опции чата");
        ui->userList_3->item(3)->setText("Сеть");
        ui->search_line_edit->setPlaceholderText("Поиск");
        ui->editText->setPlaceholderText("Введите сообщение..");
        break;
    }
    case 2:
    {
        ui->checkBox_3->setText("Chargement au lancement");
        ui->label_2->setText("Langue de: ");
        ui->label_3->setText("Le chemin d'amorçage: ");
        ui->RB_sendEnter->setText("Envoyer à \"Entrée\"");
        ui->RB_send_CEnter->setText("Envoyer à \"Ctrl + Entrée\"");
        ui->ChBox_Notif->setText("Notification");
        ui->ChBox_PSound->setText("Le son");
        ui->label_7->setText("Sélection de l'fond");
        ui->PB_SelColor->setText("Choisir de palette:");
        ui->PB_LoadFileBackground->setText("Télécharger un fichier:");
        ui->username_label->setText("Nom d'utilisateur: ");
        ui->radioButton->setText("Par défaut");
        ui->radioButton_2->setText("Download");
        ui->groupBox->setTitle("Image");
        ui->userList_3->item(0)->setText("Profil");
        ui->userList_3->item(1)->setText("Principal");
        ui->userList_3->item(2)->setText("Options du Chat");
        ui->userList_3->item(3)->setText("Networking");
        ui->search_line_edit->setPlaceholderText("Recherche en");
        ui->editText->setPlaceholderText("Inscris ton message..");
        break;
    }
    }
}

Client::~Client()
{
    delete ui;
}

