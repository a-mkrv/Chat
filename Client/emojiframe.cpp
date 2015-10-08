#include "emojiframe.h"
#include "ui_emojiframe.h"

EmojiFrame::EmojiFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::EmojiFrame)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Popup | Qt::Window);
}

EmojiFrame::~EmojiFrame()
{
    delete ui;
}
