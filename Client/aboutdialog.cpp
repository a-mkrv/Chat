#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QPropertyAnimation>


AboutDialog::AboutDialog(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AboutDialog)
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

AboutDialog::~AboutDialog()
{
    delete ui;
}
