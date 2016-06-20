#ifndef LISTDELEGATE_H
#define LISTDELEGATE_H
#include <QPainter>
#include <QAbstractItemDelegate>
#include <QString>
#include <QTextDocument>

class UserListDelegate : public QAbstractItemDelegate
{
public:
  UserListDelegate(QObject *parent = 0);
  void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // LISTDELEGATE_H
