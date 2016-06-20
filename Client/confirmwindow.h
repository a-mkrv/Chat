#ifndef CONFIRMWINDOW_H
#define CONFIRMWINDOW_H

#include <QFrame>
#include <QString>

namespace Ui {
  class ConfirmWindow;
}

class ConfirmWindow : public QFrame
{
  Q_OBJECT

public:
  explicit ConfirmWindow(QWidget *parent = 0, QString _message = 0);
  ~ConfirmWindow();

private:
  Ui::ConfirmWindow *ui;
  QString message;

signals:
  void response(QString);

private slots:
  void on_ok_Button_clicked();
  void on_cancel_Button_clicked();

public slots:
  void SetLanguage(QMap<QString, QString> &);
};

#endif // CONFIRMWINDOW_H
