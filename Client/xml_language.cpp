#include "xml_language.h"


XML_Language::XML_Language(QObject *parent) : QObject(parent)
{

}

QMap<QString, QString> XML_Language::ParseXML(QString fileName)
{
  XML_fileName = fileName;
  fileLanguage = new QFile(":/Language/Resource/XML_Language/" + XML_fileName + ".xml");

  if(!fileLanguage->open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "Language file is not found";
      return dictionary;
    }

  QXmlStreamReader xml(fileLanguage);
  QString line_tag;

  while (!xml.atEnd() && !xml.hasError() ) {
      if(xml.isStartElement())
        {
          if (xml.name() == "line")
            {
              xml.readNext();
              line_tag = xml.text().toString();
            }
          else if (xml.name() == "value")
            {
              xml.readNext();
              dictionary.insert(line_tag, xml.text().toString());
            }
        }
      xml.readNext();
    }

  return dictionary;
}
