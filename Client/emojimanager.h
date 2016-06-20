#ifndef EMOJIMANAGER_H
#define EMOJIMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>

class Emoji
{
public:
  QString symbol;
  QString description;
  QString pixmapIconNumber;
  QString category;
};

class EmojiCategory
{
public:
  QString categoryName;
  QString iconNumber;
};

class EmojiManager : public QObject
{
  Q_OBJECT
public:
  explicit EmojiManager(QObject *parent = 0);

  QList<Emoji> getEmojiList();
  QList<EmojiCategory> getEmojiCategoryList();
  QString getEmojiNumberFromSymbol(QString symbol);
  QString getEmojiSymbolFromNumber(QString number);

private:
  QStringList readText(QString fileName);
  void parseEmoticonList(QStringList content);
  QList<Emoji> emojiList;
  QList<EmojiCategory> categoryList;
  QMap<QString, QString> symbolNumberMap;
  QMap<QString, QString> numberSymbolMap;

signals:

public slots:
};

#endif // EMOJIMANAGER_H
