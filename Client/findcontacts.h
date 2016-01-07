#ifndef FINDCONTACTS_H
#define FINDCONTACTS_H

#include <QFrame>
#include <QTcpSocket>


namespace Ui {
class findcontacts;
}

class findcontacts : public QFrame
{
    Q_OBJECT

public:
    explicit findcontacts(QWidget *parent = 0);
    ~findcontacts();
     QString username;


    Ui::findcontacts *ui;

signals:
    void findUsers(QString str);

private slots:
    void recieveData(QString str);
    void on_invite_cont_button_clicked();
};

#endif // FINDCONTACTS_H
