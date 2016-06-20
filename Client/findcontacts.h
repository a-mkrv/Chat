#ifndef FINDCONTACTS_H
#define FINDCONTACTS_H

#include <QFrame>
#include <QTcpSocket>
#include <QVector>

namespace Ui {
  class findcontacts;
}

class findcontacts : public QFrame
{
  Q_OBJECT

private:
  QStringList errors;

public:
  explicit findcontacts(QWidget *parent = 0);

  Ui::findcontacts *ui;
  QString username;
  void SetErrorLayout(int _show);

  ~findcontacts();

signals:
  void findUsers(QString str);

private slots:
  void on_invite_cont_button_clicked();

public slots:
  void SetLanguage(QMap<QString, QString> &);
};

#endif // FINDCONTACTS_H
