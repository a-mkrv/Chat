#include "usersgroupinfo.h"
#include "ui_usersgroupinfo.h"
#include "userlistdelegate.h"
#include <QDebug>

/******************************************************/
/*                                                    */
/*           Information window about the user        */
/*           or the group in which the user is.       */
/*                                                    */
/******************************************************/

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// The constructor accepts parameters for the display of one the user (Name, age, city, sex)
UsersGroupInfo::UsersGroupInfo(QWidget *parent, QStringList *UserInfo) :
  QFrame(parent),
  ui(new Ui::UsersGroupInfo)
{
  // It is necessary to write about the number of files and the avatar.
  ui->setupUi(this);
  initWindow();

  ui->name_label->setText(UserInfo->at(0));
  ui->age->setText("Age: " + UserInfo->at(2));
  ui->city->setText("City: " + UserInfo->at(3));
  ui->sex->setText("Sex: " + UserInfo->at(1));
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// The constructor accepts parameters for displaying information about the group (information about the group, the number of people(list))
UsersGroupInfo::UsersGroupInfo(QWidget *parent, QString _name, QString _descr, QListWidget *list) :
  QFrame(parent),
  ui(new Ui::UsersGroupInfo)
{
  ui->setupUi(this);
  initWindow();

  ui->name_label->setText(_name);
  ui->description_label->setText(_descr);

  for(int i=0; i<list->count(); i++)
    {
      QListWidgetItem *item = new QListWidgetItem();
      item->setData(Qt::DisplayRole, list->item(i)->data(Qt::DisplayRole).toString());
      item->setData(Qt::ToolTipRole, list->item(i)->data(Qt::ToolTipRole).toString());
      item->setData(Qt::UserRole + 1, list->item(i)->data(Qt::UserRole + 1).toString());
      item->setData(Qt::DecorationRole, list->item(i)->data(Qt::DecorationRole));
      ui->userList->addItem(item);
    }

  ui->userList->setItemDelegate(new UserListDelegate(ui->userList));
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Setting animation when opening window
void UsersGroupInfo::initWindow()
{
  //To hide the edges of the form and standard buttons.
  this->setWindowFlags(Qt::Popup | Qt::Window);
  setWindowOpacity(0);
  show();

  QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
  animation->setDuration(500);
  animation->setStartValue(0);
  animation->setEndValue(1);
  animation->start();
}

UsersGroupInfo::~UsersGroupInfo()
{
  delete ui;
}
