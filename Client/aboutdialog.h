#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QFrame>

namespace Ui {
  class AboutDialog;
}

class AboutDialog : public QFrame
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget *parent = 0);
  ~AboutDialog();
  Ui::AboutDialog *ui;
private:

};

#endif // ABOUTDIALOG_H
