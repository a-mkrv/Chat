#ifndef CHOICECREATE_H
#define CHOICECREATE_H

#include <QFrame>
#include <QPropertyAnimation>


namespace Ui {
  class ChoiceCreate;
}

class ChoiceCreate : public QFrame
{
  Q_OBJECT

public:
  explicit ChoiceCreate(QWidget *parent = 0);
  ~ChoiceCreate();

private slots:
  void on_next_choice_clicked();
  void on_close_choice_clicked();

signals:
  void choice(QString);

public slots:
  void SetLanguage(QMap<QString, QString> &);

private:
  Ui::ChoiceCreate *ui;
};

#endif // CHOICECREATE_H
