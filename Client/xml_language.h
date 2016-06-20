#ifndef XML_LANGUAGE_H
#define XML_LANGUAGE_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QXmlStreamReader>

class XML_Language : public QObject
{
  Q_OBJECT
public:
  explicit XML_Language(QObject *parent = 0);
  QMap<QString, QString> ParseXML(QString);

private:
  QString XML_fileName;
  QFile *fileLanguage;
  QMap<QString, QString> dictionary;

};

#endif // XML_LANGUAGE_H
