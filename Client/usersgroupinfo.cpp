#include "usersgroupinfo.h"
#include "ui_usersgroupinfo.h"
#include "userlistdelegate.h"
#include <QDebug>

UsersGroupInfo::UsersGroupInfo(QWidget *parent, QStringList *UserInfo) :
    QFrame(parent),
    ui(new Ui::UsersGroupInfo)
{
    // Конструктор принимает список данных о пользователе (Имя, Возраст, Город, пол)
    // Нужно дописать о кол-во файлах и аватар.

    ui->setupUi(this);
    initWindow();

    ui->name_label->setText(UserInfo->at(0));
    ui->age->setText("Age: " + UserInfo->at(2));
    ui->city->setText("City: " + UserInfo->at(3));
    ui->sex->setText("Sex: " + UserInfo->at(1));
}

UsersGroupInfo::UsersGroupInfo(QWidget *parent, QString _name, QString _descr, QListWidget *list) :
    QFrame(parent),
    ui(new Ui::UsersGroupInfo)
{
    // Этот конструктор принимает инфу о группе, и список пользователей, которые в ней состоят.

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

void UsersGroupInfo::initWindow()
{
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
