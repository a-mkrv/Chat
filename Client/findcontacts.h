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

    Ui::findcontacts *ui;
    QString username;
    void SetErrorLayout(int _show);

    ~findcontacts();

signals:
    void findUsers(QString str);

private slots:
    void on_invite_cont_button_clicked();

public slots:
    void set_lang(QMap<QString, QString> &);
};

#endif // FINDCONTACTS_H
