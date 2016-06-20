#include "confirmwindow.h"
#include "ui_confirmwindow.h"
#include <QPropertyAnimation>

/******************************************************/
/*                                                    */
/*            The confirmation screen to delete       */
/*                  a user from the list,             */
/*              or delete messages from the chat.     */
/*                                                    */
/******************************************************/

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


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Confirm selection
void ConfirmWindow::on_ok_Button_clicked()
{
  emit response(QString("OK"));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Cancel selection
void ConfirmWindow::on_cancel_Button_clicked()
{
  emit response(QString("Cancel"));
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Language setting.
void ConfirmWindow::SetLanguage(QMap<QString, QString> & lan_dict)
{
  ui->cancel_Button->setText(lan_dict.value("cancel_Button_conf"));
  ui->ok_Button->setText(lan_dict.value(ui->ok_Button->objectName()));
}

ConfirmWindow::~ConfirmWindow()
{
  delete ui;
}
