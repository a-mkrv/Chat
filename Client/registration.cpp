#include "registration.h"
#include "ui_registration.h"

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    reg = new NewContact(parent);
    passhash = new HashMD5;

    socket->abort();
    socket->connectToHost("127.0.0.1", 55155);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButtonSendUser()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(close()));
    connect(reg, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));

    ui->error_label->hide();
    ui->errorconnect_label->hide();
    ui->pass_enter->setEchoMode(QLineEdit::Password);

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    this->show();
}

void registration::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void registration::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void registration::onButtonSendUser()
{
    emit sendFindContact(ui->pass_enter->text().simplified());
}

void registration::on_pushButton_clicked()
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


void registration::getMessage()
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


void registration::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Return:
        on_pushButton_clicked();
        break;
    }
}

void registration::on_reg_button_clicked()
{
    if(socket->state()!=QAbstractSocket::ConnectedState)
        ui->errorconnect_label->show();
    else
    {
        ui->errorconnect_label->hide();
        reg->show();
        this->hide();
    }
}


void registration::recieveData(QString str)
{
    if(str=="Show")
        this->show();
}

void registration::on_closeregBut_clicked()
{
    this->close();
}

void registration::on_minimazregBut_clicked()
{
    this->showMinimized();
}

registration::~registration()
{
    delete ui;
}
