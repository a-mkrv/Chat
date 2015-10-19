#include "registration.h"
#include "ui_registration.h"

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);
ok = false;
    ui->pass_label->setBuddy(ui->pass_enter);
    ui->pass_enter->setEchoMode(QLineEdit::Password);
}

registration::~registration()
{
    delete ui;
}

QString registration::on_pushButton_clicked()
{
    ok = true;
    return ui->username_enter->text();
    this->hide();
}

