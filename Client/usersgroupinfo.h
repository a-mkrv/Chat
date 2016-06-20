#ifndef USERSGROUPINFO_H
#define USERSGROUPINFO_H

#include <QFrame>
#include <QPropertyAnimation>
#include <QStringList>
#include <QListWidget>


namespace Ui {
  class UsersGroupInfo;
}

class UsersGroupInfo : public QFrame
{
  Q_OBJECT

public:
  UsersGroupInfo(QWidget *parent = 0, QStringList *_userData = 0);
  UsersGroupInfo(QWidget *parent = 0, QString _name = 0, QString _descr = 0, QListWidget *list = 0);
  void initWindow();
  ~UsersGroupInfo();

public slots:
  //void set_lang(QMap<QString, QString> &);

private:
  Ui::UsersGroupInfo *ui;
};

#endif // USERSGROUPINFO_H
