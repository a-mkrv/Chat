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
    ui->error_lay->hide();

    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);

    show();
    connect(ui->invite_cont_button, SIGNAL(clicked()), this, SLOT(on_invite_cont_button_clicked()));

    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

}


void findcontacts::on_invite_cont_button_clicked()
{
    username = ui->username_lineedit->text();
    emit findUsers(username);
}

void findcontacts::SetErrorLayout(int _show)
{
    switch (_show) {
    case 0:
        ui->error_lay->setText("You can not add yourself..");
        ui->error_lay->show();
        break;
    case 1:
        ui->error_lay->setText("User is not found..");
        ui->error_lay->show();
        break;
    case 2:
        ui->error_lay->setText("User is already added..");
        ui->error_lay->show();
        break;
    }
}

findcontacts::~findcontacts()
{
    delete ui;
}
