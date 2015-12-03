#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>

namespace Ui {
class registration;
}

class registration : public QWidget
{
    Q_OBJECT

public:
    explicit registration(QWidget *parent = 0);
    ~registration();
    bool ok;
    Ui::registration *ui;


signals:
    void sendData(QString str);
    void sendFindContact(QString str);


private slots:
    void onButtonSend();
    void onButtonSendUser();

private slots:
    void on_pushButton_clicked();
};

#endif // REGISTRATION_H
