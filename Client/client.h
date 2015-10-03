#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private:
    Ui::Client *ui;
};

#endif // CLIENT_H
