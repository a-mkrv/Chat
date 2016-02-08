#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include <QPair>

typedef QVector <QPair <QString, QVector<QPair <QString, QString>>>> ChatListVector;

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
    void addMessInChat(QString, QString, QString, QString);
    QVector <QPair <QString, QString>> FriendList(QString, ChatListVector&);
    void LoadChatList(QString, QString, ChatListVector&);
    void addChatTable(QString, QString);
    QString FindInDB(QString, QString);
    bool CorrectInput(QString, QString);
};

#endif // SQLITEDB_H
