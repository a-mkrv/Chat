#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include <QPair>

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(QObject *parent = 0);
    ~SQLiteDB();

private:
    QSqlDatabase myDB;

public slots:
    void AddContact(QString, QString, int, QString, QString);
    QString FindInDB(QString, QString);
    bool CorrectInput(QString, QString);
    QVector <QPair <QString, QString>> FriendList(QString);
};

#endif // SQLITEDB_H
