#ifndef NEWCONTACT_H
#define NEWCONTACT_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "rsacrypt.h"
#include "hashmd5.h"

namespace Ui {
  class Registration;
}

class Registration : public QWidget
{
  Q_OBJECT

public:
  explicit Registration(QWidget *parent = 0);
  ~Registration();

protected:
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);

private slots:
  void on_accept_button_clicked();
  void on_come_back_clicked();
  void on_close_window_clicked();
  void on_turn_window_clicked();
  void getMessagee();

public slots:
  void SetLanguage(QMap<QString, QString> &);

signals:
  void sendData(QString str);

private:
  Ui::Registration *ui;

  HashMD5 *hashmd5;
  RSACrypt *rsacrypt;
  QTcpSocket *socket;
  QPoint m_dragPosition;
  QStringList errors;
};

#endif // NEWCONTACT_H
