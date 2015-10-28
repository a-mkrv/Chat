#include "registration.h"
#include "ui_registration.h"

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButtonSend()));

    ui->pass_label->setBuddy(ui->pass_enter);
    ui->pass_enter->setEchoMode(QLineEdit::Password);
    this->show();
}

void registration::onButtonSend()
{
    emit sendData(ui->username_enter->text().simplified());
}

void registration::on_pushButton_clicked()
{
    QString str = ui->username_enter->text().simplified();
    if(str!="")
    this->hide();
}

registration::~registration()
{
    delete ui;
}
