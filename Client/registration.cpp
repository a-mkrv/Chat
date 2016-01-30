#include "registration.h"
#include "ui_registration.h"

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);

    socket = new QTcpSocket;
    socket->abort();
    socket->connectToHost("127.0.0.1", 55155);

    ui->error_label->hide();
    ui->errorconnect_label->hide();

    //reg = new NewContact(parent, 0);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButtonSendUser()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessage()));
   // connect(reg, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));

    ui->pass_enter->setEchoMode(QLineEdit::Password);
    this->show();
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
    QString password = ui->pass_enter->text().simplified();

    if(login!="" && password!="")
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);

        out << quint16(0) << QTime::currentTime() << QString("_LOG_IN_") << login << password;
        socket->write(block);
    }
}


void registration::getMessage()
{
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_4);
        QString mes;

        in >> mes;
        qDebug() << "GetReg" <<mes;

        if(mes == "Error_Login_Pass")
            ui->error_label->show();
        if(mes == "LogInOK!" && !ui->username_enter->text().simplified().isEmpty() && !ui->pass_enter->text().simplified().isEmpty())
        {
            emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified());
           // socket->abort();
            this->hide();
        }
}


void registration::keyReleaseEvent(QKeyEvent *event)
{
   switch(event->key()) {
   case Qt::Key_Return:
       if(ui->username_enter->text().simplified()!="" && ui->pass_enter->text().simplified()!="")
       {
           emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified());
           socket->deleteLater();
           //connect(socket, SIGNAL(disconnected()), this, SLOT());
          // socket->close();
           this->hide();
       }
      break;
   }
}

registration::~registration()
{
    delete ui;
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
    qDebug() << str;
   if(str=="Show")
   {
        this->show();
   }
}
