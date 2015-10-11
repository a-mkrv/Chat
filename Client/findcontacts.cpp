#include "findcontacts.h"
#include "ui_findcontacts.h"
#include <QPropertyAnimation>

findcontacts::findcontacts(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::findcontacts)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);

    show();

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
