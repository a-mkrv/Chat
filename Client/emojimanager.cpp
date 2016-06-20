#include "emojimanager.h"
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

EmojiManager::EmojiManager(QObject *parent) : QObject(parent)
{
  QString binDir = QCoreApplication::applicationDirPath();
  QString dataDir = binDir;
  dataDir = QDir::cleanPath(dataDir + "/");
  QDir dataLocation(dataDir + "/data");

  QString fileName = dataLocation.absolutePath() + "/emojilist.txt";

  QStringList listTmp = readText(fileName);

  // Insert firts category of emoji (ALL)
  EmojiCategory emojiCat;
  emojiCat.categoryName = "All";
  emojiCat.iconNumber = "093";

  categoryList << emojiCat;

  parseEmoticonList(listTmp);

}

QStringList EmojiManager::readText(QString fileName)
{
  QStringList content;

  QFile file(fileName);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      QString line;
      do {
          line = in.readLine();
          content << line;
        } while (!line.isNull());
    }

  return content;
}

void EmojiManager::parseEmoticonList(QStringList content)
{
  QString currentCategory;    // currentCategory remembers the current category for every emoji!

  for (int i = 0; i < content.count(); i++) {
      QString line = content.at(i);

      if (line.isEmpty())
        break;

      if (line[0] == ';') {
          QStringList splitLine = line.split(" = ");

          currentCategory = splitLine.at(0);
          currentCategory = currentCategory.remove(";");

          EmojiCategory emojiCat;
          emojiCat.categoryName = currentCategory;
          emojiCat.iconNumber = splitLine.at(1);

          categoryList << emojiCat;
        }
      else {
          QStringList splitLine = line.split(" = ");

          Emoji emoji;

          emoji.symbol = splitLine.at(0);
          emoji.description = splitLine.at(1);
          emoji.pixmapIconNumber = splitLine.at(2);
          emoji.category = currentCategory;

          symbolNumberMap.insert(emoji.symbol, emoji.pixmapIconNumber);
          numberSymbolMap.insert(emoji.pixmapIconNumber, emoji.symbol);

          emojiList << emoji;

          /*qDebug() << "Symbol " << splitLine.at(0);
            qDebug() << "Name " << splitLine.at(1);
            qDebug() << "Number " << splitLine.at(2);
            qDebug() << "Category " << currentCategory;
            qDebug() << "------\n";*/
        }
    }
}

QList<Emoji> EmojiManager::getEmojiList()
{
  return emojiList;
}

QList<EmojiCategory> EmojiManager::getEmojiCategoryList()
{
  return categoryList;
}

QString EmojiManager::getEmojiNumberFromSymbol(QString symbol)
{
  return symbolNumberMap[symbol];
}

QString EmojiManager::getEmojiSymbolFromNumber(QString number)
{
  return numberSymbolMap[number];
}
