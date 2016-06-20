#include "creategroup.h"
#include "ui_creategroup.h"

/******************************************************/
/*                                                    */
/*                 Window create a group              */
/*           (1 stage - without adding users).        */
/*       The group name, avatar and description.      */
/*                                                    */
/******************************************************/


CreateGroup::CreateGroup(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::CreateGroup)
{
  ui->setupUi(this);

  //To hide the edges of the form and standard buttons.
  this->setWindowFlags(Qt::Popup | Qt::Window);
  setWindowOpacity(0);
  show();

  //Setting animation when opening window
  QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
  animation->setDuration(500);
  animation->setStartValue(0);
  animation->setEndValue(1);
  animation->start();

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///If added to the group name and description, then proceed to add users (sekestkontakt class), the signal is sent.
void CreateGroup::on_create_group_clicked()
{
  if (!ui->g_name->text().isEmpty() && !ui->g_description->text().isEmpty())
    {
      emit GroupSignal(QString("Create"), ui->g_name->text(), ui->g_description->text(), "path");
      this->close();
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Cancel group creation. Close window
void CreateGroup::on_close_groupW_clicked()
{
  emit GroupSignal(QString("Close"), 0, 0, 0);
  this->close();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Adding an avatar for the group.
void CreateGroup::on_group_avatar_clicked()
{
  QString files = QFileDialog::getOpenFileName(this, tr("Select Images"), "" , tr("Images (*.jpg *jpeg *.png)"));

  if (QString::compare(files, QString()) !=0 )
    {
      QPixmap pixmap(files);
      bool vol = true;

      // Sometimes it badly works and the window disappears, to fix a bug.
      if (vol)
        {
          QIcon ButtonIcon(pixmap);
          ui->group_avatar->setIcon(ButtonIcon);
          //ui->group_avatar->setIconSize(pixmap.rect().size());
          //ui->group_avatar->setIcon();
          //ui->group_avatar->s chat_back_lab->setPixmap(QPixmap::fromImage(image));
          //            ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        }
      else
        {
          //Error
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Language setting.
void CreateGroup::SetLanguage(QMap<QString, QString> & lan_dict)
{
  ui->g_name->setPlaceholderText(lan_dict.value(ui->g_name->objectName()));
  ui->g_description->setPlaceholderText(lan_dict.value(ui->g_description->objectName()));
  ui->close_groupW->setText(lan_dict.value(ui->close_groupW->objectName()));
  ui->create_group->setText(lan_dict.value(ui->create_group->objectName()));
}

CreateGroup::~CreateGroup()
{
  //delete ui;
}
