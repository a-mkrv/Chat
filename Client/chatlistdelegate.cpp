#include "chatlistdelegate.h"
#include <QDebug>

ChatListDelegate::ChatListDelegate(QObject *parent)
{

}

void ChatListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    QRect r = option.rect;
    QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);
    QPen lineMarkedPen(QColor::fromRgb(0,90,131), 1, Qt::SolidLine);
    QPen fontPen(QColor::fromRgb(51,51,51), 1, Qt::SolidLine);
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

    //GET TITLE, DESCRIPTION AND ICON
    QIcon ic = QIcon(qvariant_cast<QIcon>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole + 1).toString();
    QString time = index.data(Qt::ToolTipRole).toString();

    if(option.state & QStyle::State_Selected){

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
        if(description=="FROM")
            painter->setBrush(QColor("#eeefff"));
        else if (description=="TO")
            painter->setBrush(QColor(255,255,235));

        painter->drawRect(r);

        //BORDER
        painter->setPen(linePen);
        painter->drawLine(r.topLeft(),r.topRight());
        painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.bottomLeft(),r.bottomRight());
        painter->drawLine(r.topLeft(),r.bottomLeft());

        painter->setPen(fontPen);
    }



    int imageSpace = 10;
    if (!ic.isNull()) {
        r = option.rect.adjusted(2, 2, -1, -1);
        ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
        imageSpace = 85;
    }

    if(description=="FROM")
    {
        //TITLE
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
        painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignLeft, title, &r);
        // TIME
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
        painter->drawText(r.left(), r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, time , &r);
    }
    else if (description == "TO")
    {
        //TITLE
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 11, QFont::Normal ) );
        painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignRight, title, &r);
        // TIME
        r = option.rect.adjusted(imageSpace, 10, -10, -27);
        painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
        painter->drawText(r.left(), r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
    }
}

QSize ChatListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    return QSize(200, 70); // very dumb value
}


