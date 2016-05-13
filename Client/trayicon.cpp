#include "trayicon.h"

/******************************************************/
/*                                                    */
/*          A pop-up window of a new message          */
/*                                                    */
/******************************************************/

TrayIcon::TrayIcon(QObject *parent) :  QSystemTrayIcon(parent)
{
   setIcon(QIcon(":/new/prefix1/Resource/logo.png"));

   this->setVisible(true);
}

