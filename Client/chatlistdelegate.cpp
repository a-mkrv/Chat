#include "chatlistdelegate.h"
#include <QDebug>

ChatListDelegate::ChatListDelegate(QObject *parent, QString _color)
{
    color = _color;
}

void ChatListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    QRect r = option.rect;

    //GET TITLE, DESCRIPTION AND ICON
    QIcon ic = QIcon(qvariant_cast<QIcon>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole + 1).toString();
    QString time = index.data(Qt::ToolTipRole).toString();
    int SizeMesBox = 0;

    static QString checkitem = 0;
    static QString tmp_checkitem = 0;
    QString newtime = 0;

    tmp_checkitem = time;
    if(option.state & QStyle::State_Selected){

        QLinearGradient gradientSelected(r.left(),r.top()+3,r.width(),r.height() - 6);
        gradientSelected.setColorAt(0.0, QColor::fromRgb(195,213,255));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(177,215,246));
        painter->setBrush(gradientSelected);
        painter->drawRoundedRect(r.left(), r.top()+3, r.width(), r.height() - 6, 5, 5);

        if(tmp_checkitem!=checkitem)
            if(time.size()>14)
            {    checkitem = time;
                newtime = checkitem.left(14) + "    Start Download";
            }

    } else {

        //BACKGROUND
        painter->setRenderHint(QPainter::Antialiasing);

        //painter->setPen("#dbdcff" );
        if(title.size() >= 25 && title.size() <=35)
            SizeMesBox = title.size()+320;
        else if(title.size()<25 && title.size()>15)
            SizeMesBox = title.size()+235;
        else if(title.size()<=15)
            SizeMesBox = title.size()+180;
        else if(title.size()> 45)
            SizeMesBox = r.width()-15;
        else if(title.size()> 35)
            SizeMesBox = title.size() + 400;

        painter->setPen(Qt::white);

        // Установка окна для сообщения.

        if(description=="FROM")
        {
            painter->setBrush(QColor("#ffffff"));
            //painter->setBrush(QColor("#eeefff"));
            painter->drawRoundedRect(r.left()+2, r.top()+3, SizeMesBox, r.height() - 6, 5, 5);
        }
        else if (description=="FROMF")
        {
            painter->setBrush(QColor("#ffffff"));
            painter->drawRoundedRect(r.left()+2, r.top()+3, SizeMesBox+55, r.height() - 6, 5, 5);
        }

        else if (description=="TO")
        {
            painter->setBrush(QColor("#effdde"));
            painter->drawRoundedRect(r.right()-SizeMesBox, r.top()+3, SizeMesBox-2, r.height() - 6, 5, 5);
        }
        else if (description=="TOF")
        {
            painter->setBrush(QColor("#effdde"));
            painter->drawRoundedRect(r.right()-SizeMesBox-55, r.top()+3, SizeMesBox+53, r.height() - 6, 5, 5);
        }

    }


    // Ужасный код, надо переписывать.. но, пока работает :D

    painter->setPen( Qt::black );
    if(description=="FROM" || description == "FROMF")
    {
        int imageSpace = 10;
        if (!ic.isNull()) {
            r = option.rect.adjusted(2, 10, -1, -10);
            ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
            imageSpace = 85;
        }

        //TITLE
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
        painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignLeft, title, &r);
        // TIME
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
        painter->drawText(r.left(), r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, time , &r);
    }
    else if (description == "TO" || description == "TOF")
    {
        int imageSpace = 10;
        if (!ic.isNull()) {
            r = option.rect.adjusted(10, 10, -10, -10);
            ic.paint(painter, r, Qt::AlignTop|Qt::AlignRight);
            imageSpace = 85;
        }

        if (description == "TOF")
        {
            //TITLE
            r = option.rect.adjusted(imageSpace, 10, -10, -27);
            painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
            painter->drawText(r.left()-75, r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, title, &r);
            // TIME
            r = option.rect.adjusted(imageSpace, 10, -10, -27);
            painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
            painter->drawText(r.left()-75, r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
        }
        else
        {
            //TITLE
            r = option.rect.adjusted(imageSpace, 10, -10, -27);
            painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
            painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, title, &r);
            // TIME
            r = option.rect.adjusted(imageSpace, 10, -10, -27);
            painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
            painter->drawText(r.left()-3, r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
        }
    }
    this->sizeHint(option, index);
}

QSize ChatListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    return QSize(200, 70); // very dumb value
}


