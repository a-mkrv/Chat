#include "trayicon.h"

TrayIcon::TrayIcon(QObject *parent) :  QSystemTrayIcon(parent)
{
   setIcon(QIcon(":/new/prefix1/Resource/profile5.png"));

   this->setVisible(true);
}

