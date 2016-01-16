#include "registration.h"
#include "ui_registration.h"

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);
    reg = new NewContact();
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButtonSend()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButtonSendUser()));
    connect(reg, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));

    ui->pass_enter->setEchoMode(QLineEdit::Password);
    this->show();
}

void registration::onButtonSend()
{
    emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified());
}

void registration::onButtonSendUser()
{
    emit sendFindContact(ui->pass_enter->text().simplified());
}

void registration::on_pushButton_clicked()
{
    QString str = ui->username_enter->text().simplified();
    if(str!="" && ui->pass_enter->text().simplified()!="")
    this->hide();
}

void registration::keyReleaseEvent(QKeyEvent *event)
{
   switch(event->key()) {
   case Qt::Key_Return:
       if(ui->username_enter->text().simplified()!="" && ui->pass_enter->text().simplified()!="")
       {
           emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified());
           this->hide();
       }
      break;
   }
}

registration::~registration()
{
    qDebug() << "Delete Reg";
    delete ui;
}

void registration::on_reg_button_clicked()
{
    reg->show();
    this->hide();
}


void registration::recieveData(QString str)
{
    qDebug() << str;
   if(str=="Show")
   {
        this->show();
   }
}
