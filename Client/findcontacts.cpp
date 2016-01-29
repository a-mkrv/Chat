#include "findcontacts.h"
#include "ui_findcontacts.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <client.h>

findcontacts::findcontacts(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::findcontacts)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);

    show();
    connect(ui->invite_cont_button, SIGNAL(clicked()), this, SLOT(on_invite_cont_button_clicked()));
   // connect(sender()->, SIGNAL(find_user_Serv(QString)), this, SLOT(recieveData(QString)));

    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");

    animation->setDuration(2000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

}

findcontacts::~findcontacts()
{
    delete ui;
}

void findcontacts::recieveData(QString str)
{

}

void findcontacts::on_invite_cont_button_clicked()
{
    username = ui->username_lineedit->text();
    emit findUsers(username);
}
