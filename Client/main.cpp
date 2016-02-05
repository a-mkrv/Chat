#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("HEHE");

    Client w;
    //w.hide();

    return a.exec();
}
