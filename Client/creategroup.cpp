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
    //delete ui;
}

void CreateGroup::on_create_group_clicked()
{
    if(!ui->g_name->text().isEmpty() && !ui->g_description->text().isEmpty())
    {
        emit GroupSignal(QString("Create"), ui->g_name->text(), ui->g_description->text(), "path");
        this->close();
    }
}

void CreateGroup::on_close_groupW_clicked()
{
    emit GroupSignal(QString("Close"), 0, 0, 0);
    this->close();
}

void CreateGroup::on_group_avatar_clicked()
{
    QString files = QFileDialog::getOpenFileName(this, tr("Select Images"), "" , tr("Images (*.jpg *jpeg *.png)"));

    if(QString::compare(files, QString())!=0)
    {
        QPixmap pixmap(files);
        bool vol = true;

        if(vol)
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
