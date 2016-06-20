#ifndef EMOJIFRAME_H
#define EMOJIFRAME_H

#include <QFrame>
#include "listwidgetemoji.h"
#include "emojimanager.h"


namespace Ui {
  class EmojiFrame;
}

class EmojiFrame : public QFrame
{
  Q_OBJECT

public:
  explicit EmojiFrame(QWidget *parent = 0, QString emojiPath = "");
  ~EmojiFrame();

private:
  Ui::EmojiFrame *ui;
  EmojiManager *emojiMan;
  QString m_emojiPath;
  void loadComboCategories();

private slots:
  void loadEmojiCategory(QString category);
  void emojiDoubleClickReceived(QString symbol);

signals:
  void sendEmoji(QString);
};

#endif // EMOJIFRAME_H
