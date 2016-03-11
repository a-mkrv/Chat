#include "confirmwindow.h"
#include "ui_confirmwindow.h"
#include <QPropertyAnimation>

ConfirmWindow::ConfirmWindow(QWidget *parent, QString _message) :
    QFrame(parent),
    ui(new Ui::ConfirmWindow)
{
    ui->setupUi(this);
    message = _message;
    ui->confirm_message->setText(message);

    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);
    show();

    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

ConfirmWindow::~ConfirmWindow()
{
    delete ui;
}

void ConfirmWindow::on_ok_Button_clicked()
{
    emit response(QString("OK"));
}

void ConfirmWindow::on_cancel_Button_clicked()
{
    emit response(QString("Cancel"));
}
