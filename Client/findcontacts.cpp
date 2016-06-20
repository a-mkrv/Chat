#include "findcontacts.h"
#include "ui_findcontacts.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <client.h>

/******************************************************/
/*                                                    */
/*          User search window on the server.         */
/*                                                    */
/******************************************************/

findcontacts::findcontacts(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::findcontacts)
{
  ui->setupUi(this);
  ui->error_lay->hide();

  //To hide the edges of the form and standard buttons.
  this->setWindowFlags(Qt::Popup | Qt::Window);
  setWindowOpacity(0);

  show();
  connect(ui->invite_cont_button, SIGNAL(clicked()), this, SLOT(on_invite_cont_button_clicked()));

  //Setting animation when opening window
  QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
  animation->setDuration(500);
  animation->setStartValue(0);
  animation->setEndValue(1);
  animation->start();

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///Enter a user name and send it (through signal) to the server via the main class.
void findcontacts::on_invite_cont_button_clicked()
{
  username = ui->username_lineedit->text();
  emit findUsers(username);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///Message from the server of the request error.
//1. You can add yourself e (find yourself).
//2. The user is not found.
//3. The user has been added (in the list)
void findcontacts::SetErrorLayout(int _show)
{
  switch (_show) {
    case 0:
      ui->error_lay->setText(errors.at(0)); //You can not add yourself..
      ui->error_lay->show();
      break;
    case 1:
      ui->error_lay->setText(errors.at(1)); //User is not found..
      ui->error_lay->show();
      break;
    case 2:
      ui->error_lay->setText(errors.at(2)); //User is already added..
      ui->error_lay->show();
      break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Language setting.
void findcontacts::SetLanguage(QMap<QString, QString> & lan_dict)
{
  ui->add_user->setText(lan_dict.value(ui->add_user->objectName()));
  ui->username_lineedit->setPlaceholderText(lan_dict.value(ui->username_lineedit->objectName()));
  ui->invite_cont_button->setText(lan_dict.value(ui->invite_cont_button->objectName()));

  errors = lan_dict.value("errors").split("//");

}

findcontacts::~findcontacts()
{
  delete ui;
}
