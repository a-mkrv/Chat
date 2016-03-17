#include "choicecreate.h"
#include "ui_choicecreate.h"

ChoiceCreate::ChoiceCreate(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChoiceCreate)
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

ChoiceCreate::~ChoiceCreate()
{
    delete ui;
}

void ChoiceCreate::on_next_choice_clicked()
{
    if(ui->newContact->isChecked())
    {
        emit choice(QString("newContact"));
        this->close();
    }

    if(ui->newGroup->isChecked())
    {
        emit choice(QString("newGroup"));
        this->close();
    }
}

void ChoiceCreate::on_close_choice_clicked()
{
    emit choice(QString("close"));
    this->close();
}
