#ifndef EMOJIFRAME_H
#define EMOJIFRAME_H

#include <QFrame>

namespace Ui {
class EmojiFrame;
}

class EmojiFrame : public QFrame
{
    Q_OBJECT

public:
    explicit EmojiFrame(QWidget *parent = 0);
    ~EmojiFrame();

private:
    Ui::EmojiFrame *ui;

private slots:

};

#endif // EMOJIFRAME_H
