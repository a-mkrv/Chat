#ifndef CHATLISTDELEGATE_H
#define CHATLISTDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

class ChatListDelegate : public QAbstractItemDelegate
{
public:
    ChatListDelegate(QObject *parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

};

#endif // CHATLISTDELEGATE_H
