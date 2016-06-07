#include "authorization.h"
#include "ui_authorization.h"

Authorization::Authorization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Authorization)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    registration = new Registration(parent);
    passhash = new HashMD5;

    socket->abort();
    socket->connectToHost("127.0.0.1", 55155);

    connect(ui->log_in, SIGNAL(clicked()), this, SLOT(onButtonSendUser()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(close()));
    connect(registration, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));

    ui->error_label->hide();
    ui->errorconnect_label->hide();
    ui->pass_enter->setEchoMode(QLineEdit::Password);

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    this->show();
}

void Authorization::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void Authorization::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Authorization::onButtonSendUser()
{
    emit sendFindContact(ui->pass_enter->text().simplified());
}

void Authorization::on_log_in_clicked()
{
    if(socket->state()!=QAbstractSocket::ConnectedState)
        ui->errorconnect_label->show();
    else
        ui->errorconnect_label->hide();

    QString login = ui->username_enter->text().simplified();
    QString password = passhash->hashSumPass(ui->pass_enter->text().simplified());

    if(!login.isEmpty() && !ui->pass_enter->text().simplified().isEmpty())
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        out << quint32(0) << QTime::currentTime() << QString("_LOG_IN_") << login << password;
        socket->write(block);
    }
}

void Authorization::getMessage()
{
    QString received_message, tmp;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_4);
    in  >> received_message >> tmp;

    qDebug() << "Get" << received_message;
    if(received_message == "Error_Login_Pass")
        ui->error_label->show();

    else if(received_message == "LogInOK!" && !ui->username_enter->text().simplified().isEmpty() && !ui->pass_enter->text().simplified().isEmpty())
    {
        emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified(), tmp);
        socket->close();
        socket->disconnectFromHost();
    }
}

void Authorization::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Return:
        on_log_in_clicked();
        break;
    }
}

void Authorization::on_reg_button_clicked()
{
    if(socket->state()!=QAbstractSocket::ConnectedState)
        ui->errorconnect_label->show();
    else
    {
        ui->errorconnect_label->hide();
        registration->show();
        this->hide();
    }
}

void Authorization::set_lang(QMap<QString, QString> & lan_dict)
{
    ui->username_enter->setPlaceholderText(lan_dict.key(ui->username_enter->objectName()));
    ui->pass_enter->setPlaceholderText(lan_dict.key(ui->pass_enter->objectName()));
    ui->error_label->setText(lan_dict.key(ui->error_label->objectName()));
    ui->reg_button->setText(lan_dict.key(ui->reg_button->objectName()));
    ui->log_in->setText(lan_dict.key(ui->log_in->objectName()));
    ui->errorconnect_label->setText(lan_dict.key(ui->errorconnect_label->objectName()));
}

void Authorization::recieveData(QString rStr)
{
    if(rStr=="Show")
        this->show();
}

void Authorization::on_close_window_clicked()
{
    this->close();
}

void Authorization::on_turn_window_clicked()
{
    this->showMinimized();
}

Authorization::~Authorization()
{
    delete ui;
}

