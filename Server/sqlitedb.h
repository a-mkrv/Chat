#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include <QPair>

typedef QList <QPair <QString, QList<QPair <QString, QString> > > > ChatListVector;

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(QObject *parent = 0);
    ~SQLiteDB();

private:
    QSqlDatabase myDB;
    QString CheckPass(QString);

public slots:
    void AddContact(QString, QString, int, QString, QString, QString, QString);
    void addMessInChat(QString, QString, QString, QString);
    QList <QPair <QString, QString> > FriendList(QString, ChatListVector&);
    QList <QPair <QString, QString> > FriendKeys(QString);
    void LoadChatList(QString, QString, ChatListVector&);
    void createGroup(QString , QString , QStringList);
    void addChatTable(QString, QString);
    QString FindInDB(QString, QString);
    void ClearHistory(QString, QString);
    void delFriend(QString, QString);
    void UpOnlineStatus(const QString&, const QString&);
    void getOnlineStatus(const QString&, QHash<QString, QString> &, QStringList &);
    QString CorrectInput(QString, QString);
    QStringList UserData(QString);
    void  FriendListName(QString, QStringList &);
};

#endif // SQLITEDB_H
