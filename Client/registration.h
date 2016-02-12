#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QKeyEvent>
#include <QTcpSocket>
#include "newcontact.h"
#include <QTime>


namespace Ui {
class registration;
}

class registration : public QWidget
{
    Q_OBJECT

public:
    explicit registration(QWidget *parent = 0);
    virtual void keyPressEvent(QKeyEvent* event) { key = event->key(); }
    virtual void keyReleaseEvent(QKeyEvent* event);

    ~registration();
    bool ok;
    Ui::registration *ui;
    NewContact *reg;
private:
    QPoint m_dragPosition;

     int key;

protected:
     QTcpSocket *socket;
     void mouseMoveEvent(QMouseEvent *event);
     void mousePressEvent(QMouseEvent *event);

signals:
    void sendData(QString str, QString pas, QString key);
    void sendFindContact(QString str);


private slots:
    void onButtonSendUser();
    void recieveData(QString str);
    void getMessage();

private slots:
    void on_pushButton_clicked();
    void on_reg_button_clicked();
    void on_closeregBut_clicked();
    void on_minimazregBut_clicked();
};

#endif // REGISTRATION_H
