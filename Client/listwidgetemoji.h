#ifndef LISTWIDGETEMOJI_H
#define LISTWIDGETEMOJI_H

#include <QWidget>
#include <QListWidget>
#include <QMap>
#include <QDir>
#include <QCoreApplication>

#include "emojimanager.h"

class ListWidgetEmoji : public QListWidget
{
  Q_OBJECT
public:
  explicit ListWidgetEmoji(QWidget *parent = 0);
  void loadEmojiListView(QString category);
  void setEmojiManager(EmojiManager *emojiMan);

private:
  EmojiManager *m_emojiMan;
  QList<Emoji> m_emojiList;
  QMap<QString, int> map;    // Description / index

signals:
  void sendEmoji(QString);

public slots:

private slots:
  void getCurrentIndex(QListWidgetItem *item);
};


#endif // LISTWIDGETEMOJI_H
