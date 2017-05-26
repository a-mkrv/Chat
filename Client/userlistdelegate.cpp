#include "userlistdelegate.h"
#include <QLabel>
#include <QDebug>

/******************************************************/
/*                                                    */
/*    UserListDelegate changes the standard display   */
/*  of users to display the name of the last message, */
/*                  time and avatars.                 */
/*  And all this allows to combine in a single cell.  */
/*                                                    */
/******************************************************/

UserListDelegate::UserListDelegate(QObject *parent)
{

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// painter - responsible for drawing.
/// option  - display area
/// index   - takes the input parameters:
//  Sender's name                   - title
//  The last received message       - description
//  Time                            - time
//  Image                           - mediaIcon (User Avatar)

void UserListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{

  // Get title, description and icon
  QIcon mediaIcon = QIcon(qvariant_cast<QIcon>(index.data(Qt::DecorationRole)));
  QString title = index.data(Qt::DisplayRole).toString();
  QString description = index.data(Qt::UserRole + 1).toString();
  QString time = index.data(Qt::ToolTipRole).toString();

  QRect r = option.rect;
  QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);
  QPen lineMarkedPen(QColor::fromRgb(0,90,131), 1, Qt::SolidLine);
  QPen fontPen(QColor::fromRgb(51,51,51), 1, Qt::SolidLine);
  QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

  // If a user is selected (pressed), changing the color(background) and size of the border area
  if (option.state & QStyle::State_Selected){

      QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());
      gradientSelected.setColorAt(0.0, QColor::fromRgb(195,213,255));
      gradientSelected.setColorAt(1.0, QColor::fromRgb(177,215,246));
      painter->setBrush(gradientSelected);
      painter->drawRect(r);

      //BORDER
      painter->setPen(lineMarkedPen);
      painter->drawLine(r.topLeft(),r.topRight());
      painter->drawLine(r.topRight(),r.bottomRight());
      painter->drawLine(r.bottomLeft(),r.bottomRight());
      painter->drawLine(r.topLeft(),r.bottomLeft());

      painter->setPen(fontMarkedPen);

    } else {

      //BACKGROUND
      //ALTERNATING COLORS
      painter->setBrush( (index.row() % 2) ? Qt::white : QColor(252,252,252) );
      painter->drawRect(r);

      //BORDER
      painter->setPen(linePen);
      painter->drawLine(r.topLeft(),r.topRight());
      painter->drawLine(r.topRight(),r.bottomRight());
      painter->drawLine(r.bottomLeft(),r.bottomRight());
      painter->drawLine(r.topLeft(),r.bottomLeft());

      painter->setPen(fontPen);
    }

  //  The offset for the installation image
  int imageSpace = 10;
  if (!mediaIcon.isNull())
    {
      r = option.rect.adjusted(2, 2, -1, -1);
      mediaIcon.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
      imageSpace = 85;
    }

  //TITLE
  r = option.rect.adjusted(imageSpace, 10, -10, -27);
  painter->setFont( QFont( "Lucida Grande", 16, QFont::Normal ) );
  painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignLeft, title, &r);
  // TIME
  r = option.rect.adjusted(60, 15, -10, -27);
  painter->setFont( QFont( "Lucida Grande", 12, QFont::Normal ) );
  painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, time , &r);
  //DESCRIPTION
  r = option.rect.adjusted(imageSpace, 35, -10, 10);
  painter->setFont( QFont( "Lucida Grande", 12, QFont::Normal ) );

  //If the last message is long, it is cut, for convenient display
  if (description.size() > 30)
    {
      description.remove(30, description.size());
      description.append("...");
    }
  painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, description, &r);

}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// Set the message block size
QSize UserListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
  return QSize(200, 70);
}


