#ifndef FINDCONTACTS_H
#define FINDCONTACTS_H

#include <QFrame>

namespace Ui {
class findcontacts;
}

class findcontacts : public QFrame
{
    Q_OBJECT

public:
    explicit findcontacts(QWidget *parent = 0);
    ~findcontacts();

private:
    Ui::findcontacts *ui;
};

#endif // FINDCONTACTS_H
