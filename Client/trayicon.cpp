#include "trayicon.h"

TrayIcon::TrayIcon(QObject *parent) :  QSystemTrayIcon(parent)
{
   setIcon(QIcon(":/new/prefix1/Resource/logo.png"));

   this->setVisible(true);
}

