#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include <QPair>

typedef QList <QPair <QString, QList<QPair <QString, QString> > > > ChatListVector;
typedef QList <QPair <QString, QString>> PairStringList;

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
    QString FriendList(QString, int &);
    QString getChatHistory(QString user);
    QString getChatHistoryPerUser(QString myName, QString user);
    void createGroup(QString , QString , QStringList);
    void addChatTable(QString, QString);
    QString FindInDB(QString, QString);
    void ClearHistory(QString, QString);
    void delFriend(QString, QString);
    void UpOnlineStatus(const QString&, const QString&);
    void updateAllDataOfUser(QStringList);
    void updateStateNotificationFromUser(QString, QString, QString);
    QStringList getOnlineStatus(const QString & user_name);
    QString getFullUserInformations(QString);
    QString getOnlyPublicKey(QString);
    QString CorrectInput(QString, QString);
    QStringList UserData(QString);
    void  FriendListName(QString, QStringList &);
};

#endif // SQLITEDB_H
