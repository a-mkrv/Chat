#include "listwidgetemoji.h"

ListWidgetEmoji::ListWidgetEmoji(QWidget *parent) :
  QListWidget(parent)
{
  connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(getCurrentIndex(QListWidgetItem *)));
}

void ListWidgetEmoji::loadEmojiListView(QString category)
{
  m_emojiList = m_emojiMan->getEmojiList();

  for (int i = m_emojiList.count() - 1; i >= 0; i--) {
      if (category != "All") {
          if (category != m_emojiList.at(i).category)
            continue;
        }

      QListWidgetItem *newItem = new QListWidgetItem;

      QString binDir = QCoreApplication::applicationDirPath();
      QString dataDir = binDir;
      dataDir = QDir::cleanPath(dataDir + "/");

      QDir iconsLocation(dataDir + "/data/icons");

      QIcon ico;
      QString iconFileName = QString("%1/x-%2.png")
          .arg(iconsLocation.absolutePath())
          .arg(m_emojiList.at(i).pixmapIconNumber);
      QPixmap p(iconFileName);
      ico.addPixmap(p);
      newItem->setIcon(ico);

      newItem->setData(Qt::ToolTipRole, m_emojiList.at(i).description);

      insertItem(0, newItem);

      map.insert(m_emojiList.at(i).description, i);    // Description | Index
    }
}

void ListWidgetEmoji::setEmojiManager(EmojiManager *emojiMan)
{
  m_emojiMan = emojiMan;
}

void ListWidgetEmoji::getCurrentIndex(QListWidgetItem *item)
{
  QString descr = item->data(Qt::ToolTipRole).toString();
  int index = map[descr];    // Get the index using description

  emit sendEmoji(m_emojiList.at(index).symbol);
}
