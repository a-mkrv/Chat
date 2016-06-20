#include "emojiframe.h"
#include "ui_emojiframe.h"

EmojiFrame::EmojiFrame(QWidget *parent, QString emojiPath) :
  QFrame(parent),
  ui(new Ui::EmojiFrame)
{
  ui->setupUi(this);
  this->setWindowFlags(Qt::Popup | Qt::Window);

  m_emojiPath = emojiPath;

  emojiMan = new EmojiManager();

  loadComboCategories();
  connect(ui->comboEmojiCategory, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadEmojiCategory(QString)));
  connect(ui->listWidgetEmoji, SIGNAL(sendEmoji(QString)), this, SLOT(emojiDoubleClickReceived(QString)));

  ui->listWidgetEmoji->setEmojiManager(emojiMan);
  ui->listWidgetEmoji->loadEmojiListView("All");
}

EmojiFrame::~EmojiFrame()
{
  delete ui;
}

void EmojiFrame::loadComboCategories()
{
  QList<EmojiCategory> eCatList = emojiMan->getEmojiCategoryList();

  for (int i = 0; i < eCatList.count(); i++) {
      QString name = eCatList.at(i).categoryName;
      QString iconNumber = eCatList.at(i).iconNumber;

      QIcon ico;
      QString iconFileName = QString("%1/x-%2.png")
          .arg(m_emojiPath)
          .arg(iconNumber);
      QPixmap p(iconFileName);
      ico.addPixmap(p);

      ui->comboEmojiCategory->addItem(p, name);
    }
}

void EmojiFrame::loadEmojiCategory(QString category)
{
  ui->listWidgetEmoji->clear();
  ui->listWidgetEmoji->loadEmojiListView(category);
}

void EmojiFrame::emojiDoubleClickReceived(QString symbol)
{
  emit sendEmoji(symbol);
}
