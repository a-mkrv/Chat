#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private:
    Ui::Server *ui;
};

#endif // SERVER_H
