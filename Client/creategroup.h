#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QFrame>
#include <QPropertyAnimation>
#include <QFileDialog>

namespace Ui {
  class CreateGroup;
}

class CreateGroup : public QFrame
{
  Q_OBJECT

public:
  explicit CreateGroup(QWidget *parent = 0);
  ~CreateGroup();

private slots:
  void on_create_group_clicked();
  void on_close_groupW_clicked();
  void on_group_avatar_clicked();

signals:
  void GroupSignal(QString, QString, QString, QString);

public slots:
  void SetLanguage(QMap<QString, QString> &);

private:
  Ui::CreateGroup *ui;
};

#endif // CREATEGROUP_H
