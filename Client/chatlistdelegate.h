#ifndef CHATLISTDELEGATE_H
#define CHATLISTDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

class ChatListDelegate : public QAbstractItemDelegate
{
public:
  ChatListDelegate(QObject *parent = 0, QString _color = 0);
  void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
  QString color;

};

#endif // CHATLISTDELEGATE_H
