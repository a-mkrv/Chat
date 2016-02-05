#include "sqlitedb.h"

SQLiteDB::SQLiteDB(QObject *parent) : QObject(parent)
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");

    QString pathToDB = QString("C:/Users/Makarov/Desktop/mydatabase.sqlite");
    myDB.setDatabaseName(pathToDB);

    QFileInfo checkFile(pathToDB);
    if (checkFile.isFile())
        if (myDB.open())
            qDebug() << "[+] Connected to Database File";

        else
            qDebug() <<"[!] Database File does not exist";

}


SQLiteDB::~SQLiteDB()
{
    qDebug() << "Closing the connection to Database file on exist";
    myDB.close();
}

void SQLiteDB::AddContact(QString UserName, QString Sex, int Age, QString City, QString Pas)
{
    QSqlQuery query(myDB);
    query.prepare("INSERT INTO Users (UserName, Sex, Age, City, Password) VALUES (:UserName, :Sex, :Age, :City, :Password)");
    query.bindValue(":UserName", UserName);
    query.bindValue(":Sex", Sex);
    query.bindValue(":Age", Age);
    query.bindValue(":City", City);
    query.bindValue(":Password", Pas);
    query.exec();

    QSqlQuery queryCreate(myDB);
    queryCreate.exec("CREATE TABLE Friend" + UserName + " (name text NOT NULL, sex text NOT NULL)");
}

QString SQLiteDB::FindInDB(QString UserName, QString whoFind)
{
    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Sex FROM Users WHERE UserName=\'" +UserName+ "\'"))
        if(query.next())
            if (query.value(0).toString()==UserName)
            {
                if(!whoFind.isEmpty())
                {
                       QSqlQuery queryAdFr(myDB);
                       queryAdFr.prepare("INSERT INTO Friend"+whoFind+" (name, sex) VALUES (:UserName, :Sex)");
                       queryAdFr.bindValue(":UserName", UserName);
                       queryAdFr.bindValue(":Sex", query.value(1).toString());
                       queryAdFr.exec();
                }
                return query.value(1).toString();
            }

    query.exec();
    return "false";
}

bool SQLiteDB::CorrectInput(QString _login, QString _password)
{
    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Password FROM Users WHERE UserName=\'" +_login+ "\' AND Password=\'"+_password+ "\'"))
        if(query.next())
            if (query.value(0).toString()== _login && query.value(1).toString()==_password)
                return true;   

    query.exec();
    return false;
}

QVector <QPair<QString, QString>> SQLiteDB::FriendList(QString user)
{
    QSqlQuery query(myDB);
    QVector <QPair <QString, QString>> FriendSex;

    if(query.exec("SELECT name, sex FROM Friend" + user))
        while (query.next())
            FriendSex.push_back(qMakePair(query.value(0).toString(), query.value(1).toString()));

    return FriendSex;
}
