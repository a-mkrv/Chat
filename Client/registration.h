#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QWidget>

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
    QString on_pushButton_clicked();

};

#endif // REGISTRATION_H
