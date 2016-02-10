#include "chatlistdelegate.h"
#include <QDebug>

ChatListDelegate::ChatListDelegate(QObject *parent, QString _color)
{
color = _color;
qDebug() << color;
}

void ChatListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    QRect r = option.rect;

    //GET TITLE, DESCRIPTION AND ICON
    QIcon ic = QIcon(qvariant_cast<QIcon>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole + 1).toString();
    QString time = index.data(Qt::ToolTipRole).toString();

    if(option.state & QStyle::State_Selected){

        QLinearGradient gradientSelected(r.left(),r.top()+3,r.width(),r.height() - 6);
        gradientSelected.setColorAt(0.0, QColor::fromRgb(195,213,255));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(177,215,246));
        painter->setBrush(gradientSelected);
        painter->drawRoundedRect(r.left(), r.top()+3, r.width(), r.height() - 6, 5, 5);


    } else {

        //BACKGROUND
        painter->setRenderHint(QPainter::Antialiasing);

        //painter->setPen("#dbdcff" );
        int SizeMesBox = 0;
        if(title.size() >= 25 && title.size() <=35)
            SizeMesBox = title.size()+320;
        else if(title.size()<25)
            SizeMesBox = title.size()+170;
        else if(title.size()> 45)
            SizeMesBox = r.width()-15;
        else if(title.size()> 35)
            SizeMesBox = title.size() + 400;

        painter->setPen(Qt::white);

        // Установка окна для сообщения.

        if(description=="FROM")
        {
            painter->setBrush(QColor("#eeefff"));
            painter->drawRoundedRect(r.left()+2, r.top()+3, SizeMesBox, r.height() - 6, 5, 5);
        }
        else if (description=="TO")
        {
            painter->setBrush(QColor("#effdde"));
            painter->drawRoundedRect(r.right()-SizeMesBox, r.top()+3, SizeMesBox-2, r.height() - 6, 5, 5);
        }
    }

    int imageSpace = 10;
    if (!ic.isNull()) {
        r = option.rect.adjusted(2, 2, -1, -1);
        ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
        imageSpace = 85;
    }

    painter->setPen( Qt::black );
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
        painter->drawText(r.left()-3, r.top()+10, r.width(), r.height(), Qt::AlignBottom|Qt::AlignRight, time , &r);
    }

}

QSize ChatListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    return QSize(200, 70); // very dumb value
}


