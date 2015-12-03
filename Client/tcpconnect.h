#ifndef TCPCONNECT_H
#define TCPCONNECT_H

#include <QObject>
#include "ui_maingui.h"

class tcpconnect : public QObject
{
    Q_OBJECT
public:
    explicit tcpconnect(QObject *parent = 0);
    ~tcpconnect();

signals:

private:


public slots:
};

#endif // TCPCONNECT_H
