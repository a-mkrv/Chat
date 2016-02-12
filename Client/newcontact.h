#ifndef NEWCONTACT_H
#define NEWCONTACT_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include "rsacrypt.h"

namespace Ui {
class NewContact;
}

class NewContact : public QWidget
{
    Q_OBJECT

public:
    explicit NewContact(QWidget *parent = 0);
    ~NewContact();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_accept_button_clicked();
    void getMessagee();
    void on_pushButton_clicked();
    void on_closeregBut_clicked();
    void on_minimazregBut_clicked();

signals:
    void sendData(QString str);

private:
    RSACrypt *rsacrypt;
    QTcpSocket *socket;
    Ui::NewContact *ui;
    QPoint m_dragPosition;
};

#endif // NEWCONTACT_H
