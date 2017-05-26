#include "chatlistdelegate.h"
#include <QDebug>

/******************************************************/
/*                                                    */
/* ChatListDelegate replaces the standard display a   */
/*  list of chat and allows combining text and images */
/*                                                    */
/******************************************************/

ChatListDelegate::ChatListDelegate(QObject *parent, QString _color)
{
  color = _color;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// painter - responsible for drawing.
/// option  - display area
/// index   - takes the input parameters:
//  Sender's name                   - title
//  Message text                    - description
//  Definition (who and to whom)    - T/F
//  Time                            - time
//  Image                           - mediaIcon (at the transmit / receive file)

void ChatListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{
  QRect r = option.rect;

  // Get title, description and icon
  QIcon mediaIcon = QIcon(qvariant_cast<QIcon>(index.data(Qt::DecorationRole)));
  QString title = index.data(Qt::DisplayRole).toString();
  QString description = index.data(Qt::UserRole + 1).toString();
  QString time = index.data(Qt::ToolTipRole).toString();
  int SizeMesBox = 0;

  static QString checkitem = 0;
  static QString tmp_checkitem = 0;
  QString newtime = 0;

  tmp_checkitem = time;

  // If a message is selected (pressed), changing the color(background) and size of the border area
  if (option.state & QStyle::State_Selected)
    {
      QLinearGradient gradientSelected(r.left(),r.top()+3,r.width(),r.height() - 6);
      gradientSelected.setColorAt(0.0, QColor::fromRgb(195,213,255));
      gradientSelected.setColorAt(1.0, QColor::fromRgb(177,215,246));
      painter->setBrush(gradientSelected);
      painter->drawRoundedRect(r.left(), r.top()+3, r.width(), r.height() - 6, 5, 5);

      if (tmp_checkitem != checkitem)
        if (time.size() > 14)
          {   checkitem = time;
            newtime = checkitem.left(14) + "    Start Download";
          }

    } else {

      //Background
      painter->setRenderHint(QPainter::Antialiasing);

      // Setting the right message, indents

      //painter->setPen("#dbdcff" );
      if (title.size() >= 25 && title.size() <= 35)
        SizeMesBox = title.size()+320;
      else if (title.size() < 25 && title.size() > 15)
        SizeMesBox = title.size() + 235;
      else if (title.size() <=15)
        SizeMesBox = title.size() + 180;
      else if (title.size() > 45)
        SizeMesBox = r.width() - 15;
      else if (title.size() > 35)
        SizeMesBox = title.size() + 400;

      painter->setPen(Qt::white);

      // Setting the message box.
      if (description == "FROM")
        {
          // #eeefff - Pale blue  (From message)
          // #effdde - Pale green (To message)
          painter->setBrush(QColor("#d5d7ff"));
          painter->drawRoundedRect(r.left()+2, r.top()+3, SizeMesBox, r.height() - 6, 5, 5);
        }
      else if (description == "FROMF")
        {
          //painter->setBrush(QColor("#eeefff"));
          painter->drawRoundedRect(r.left()+2, r.top()+3, SizeMesBox+55, r.height() - 6, 5, 5);
        }

      else if (description == "TO")
        {
          painter->setBrush(QColor("#e3fcc6"));
          painter->drawRoundedRect(r.right()-SizeMesBox, r.top()+3, SizeMesBox-2, r.height() - 6, 5, 5);
        }
      else if (description == "TOF")
        {
          //painter->setBrush(QColor("#effdde"));
          painter->drawRoundedRect(r.right()-SizeMesBox-55, r.top()+3, SizeMesBox+53, r.height() - 6, 5, 5);
        }

    }


  painter->setPen( Qt::black );
  if (description == "FROM" || description == "FROMF")
    {
      //  The offset for the installation image
      int imageSpace = 10;
      if (!mediaIcon.isNull()) {
          r = option.rect.adjusted(2, 10, -1, -10);
          mediaIcon.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
          imageSpace = 85;
        }

      //TITLE
      r = option.rect.adjusted(imageSpace, 10, -10, -27);
      painter->setFont( QFont( "Lucida Grande", 16, QFont::Normal ) );
      painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignLeft, title, &r);

      // TIME
      r = option.rect.adjusted(imageSpace, 10, -10, -27);
      painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
      painter->drawText(r.left(), r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, time , &r);
    }
  else if (description == "TO" || description == "TOF")
    {
      int imageSpace = 20;
      if (!mediaIcon.isNull()) {
          r = option.rect.adjusted(10, 10, -10, -10);
          mediaIcon.paint(painter, r, Qt::AlignTop|Qt::AlignRight);
          imageSpace = 85;
        }

      if (description == "TOF")
        {
          //TITLE
          r = option.rect.adjusted(imageSpace, 10, -20, -27);
          painter->setFont( QFont( "Lucida Grande", 16, QFont::Normal ) );
          painter->drawText(r.left()-75, r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, title, &r);
          // TIME
          r = option.rect.adjusted(imageSpace, 10, -20, -27);
          painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
          painter->drawText(r.left()-75, r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
        }
      else
        {
          //TITLE
          r = option.rect.adjusted(imageSpace, 10, -20, -27);
          painter->setFont( QFont( "Lucida Grande", 16, QFont::Normal ) );
          painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, title, &r);
          // TIME
          r = option.rect.adjusted(imageSpace, 10, -20, -27);
          painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
          painter->drawText(r.left()-3, r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
        }
    }
  this->sizeHint(option, index);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Set the message block size
QSize ChatListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
  return QSize(200, 70); // very dumb value
}


