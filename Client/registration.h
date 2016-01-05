#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QKeyEvent>


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

private:
     int key;
signals:
    void sendData(QString str, QString pas);
    void sendFindContact(QString str);


private slots:
    void onButtonSend();
    void onButtonSendUser();

private slots:
    void on_pushButton_clicked();
};

#endif // REGISTRATION_H
