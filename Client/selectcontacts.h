#ifndef SELECTCONTACTS_H
#define SELECTCONTACTS_H

#include <QFrame>
#include <QPropertyAnimation>
#include <QListWidget>
#include <QStringList>
#include "userlistdelegate.h"

namespace Ui {
  class SelectContacts;
}

class SelectContacts : public QFrame
{
  Q_OBJECT

public:
  explicit SelectContacts(QWidget *parent = 0, QListWidget *list = 0);
  ~SelectContacts();

signals:
  void SelectUsersSignal(QStringList, QString);

private slots:
  void on_crGroup_button_clicked();
  void on_cancel_button_clicked();
  void on_userList_itemSelectionChanged();

public slots:
  void SetLanguage(QMap<QString, QString> &);

private:
  Ui::SelectContacts *ui;
  QString languageMembers;
};

#endif // SELECTCONTACTS_H
