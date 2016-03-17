#include "creategroup.h"
#include "ui_creategroup.h"

CreateGroup::CreateGroup(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);
    show();

    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

}

CreateGroup::~CreateGroup()
{
    delete ui;
}

void CreateGroup::on_create_group_clicked()
{
    if(!ui->g_name->text().isEmpty() && !ui->g_description->text().isEmpty())
    {
        emit groupSig(QString("Create"));
        this->close();
    }
}

void CreateGroup::on_close_groupW_clicked()
{
    emit groupSig(QString("Close"));
    this->close();
}
