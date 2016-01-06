#ifndef LISTDELEGATE_H
#define LISTDELEGATE_H
#include <QPainter>
#include <QAbstractItemDelegate>
#include <QString>

class ListDelegate : public QAbstractItemDelegate
{
public:
    ListDelegate(QObject *parent = 0, QString _date=0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QString date;
    virtual ~ListDelegate();
};

#endif // LISTDELEGATE_H
