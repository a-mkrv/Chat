#include "selectcontacts.h"
#include "ui_selectcontacts.h"
#include <QDebug>


/******************************************************/
/*                                                    */
/* Window selection contacts when added to the group. */
/*      Displaying a list of users from the user      */
/*       who creates a group and can add people       */
/*            from your list of friends.              */
/*                                                    */
/******************************************************/

SelectContacts::SelectContacts(QWidget *parent, QListWidget *list) :
  QFrame(parent),
  ui(new Ui::SelectContacts)
{
  ui->setupUi(this);

  //To hide the edges of the form and standard buttons.
  this->setWindowFlags(Qt::Popup | Qt::Window);
  setWindowOpacity(0);
  show();

  // Setting animation when opening window
  QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
  animation->setDuration(500);
  animation->setStartValue(0);
  animation->setEndValue(1);
  animation->start();

  ui->userList->setItemDelegate(new UserListDelegate(ui->userList));

  for (int i = 0; i < list->count(); i++)
    {
      QListWidgetItem *item = new QListWidgetItem();
      item->setData(Qt::DisplayRole, list->item(i)->data(Qt::DisplayRole).toString());
      item->setData(Qt::ToolTipRole, list->item(i)->data(Qt::ToolTipRole).toString());
      item->setData(Qt::UserRole + 1, list->item(i)->data(Qt::UserRole + 1).toString());
      item->setData(Qt::DecorationRole, list->item(i)->data(Qt::DecorationRole));
      ui->userList->addItem(item);
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Members are selected. In the main class is sent a signal to create a group.
void SelectContacts::on_crGroup_button_clicked()
{
  QStringList userList;
  if (!ui->userList->selectedItems().isEmpty())
    {
      for (int i = 0; i<ui->userList->count(); i++)
        {
          if (ui->userList->item(i)->isSelected())
            userList.push_back(ui->userList->item(i)->data(Qt::DisplayRole).toString());
        }

      emit SelectUsersSignal(userList, "Create");
      this->close();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Cancel group creation. Close window
void SelectContacts::on_cancel_button_clicked()
{
  QStringList tmp;
  emit SelectUsersSignal(tmp, "Cancel");
  this->close();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Counter users that are added to the group. On-screen display
void SelectContacts::on_userList_itemSelectionChanged()
{
  int count = 0;
  for (int i = 0; i < ui->userList->count(); i++)
    {
      if (ui->userList->item(i)->isSelected())
        count ++;
    }

  ui->count_member->setText(languageMembers + QString::number(count) + "/" + QString::number(ui->userList->count()));
}

void SelectContacts::SetLanguage(QMap<QString, QString> & lan_dict)
{
  languageMembers = lan_dict.value(ui->count_member->objectName());
  ui->count_member->setText(languageMembers + "0" + "/" + QString::number(ui->userList->count()));
  ui->crGroup_button->setText(lan_dict.value(ui->crGroup_button->objectName()));
  ui->cancel_button->setText(lan_dict.value(ui->cancel_button->objectName()));
}

SelectContacts::~SelectContacts()
{
  //delete ui;
}
