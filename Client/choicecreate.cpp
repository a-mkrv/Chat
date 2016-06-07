#include "choicecreate.h"
#include "ui_choicecreate.h"

/******************************************************/
/*                                                    */
/*            Selection window between adding         */
/*             a contact or group creation            */
/*                                                    */
/******************************************************/

ChoiceCreate::ChoiceCreate(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChoiceCreate)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Popup | Qt::Window);
    setWindowOpacity(0);

    show();

    // Setting animation when opening window
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

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Choice of a variant
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

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Close window
void ChoiceCreate::on_close_choice_clicked()
{
    emit choice(QString("close"));
    this->close();
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///
void ChoiceCreate::set_lang(QMap<QString, QString> &lan_dict)
{
   ///////
}
