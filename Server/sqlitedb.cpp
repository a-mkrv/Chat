#include "sqlitedb.h"

SQLiteDB::SQLiteDB(QObject *parent) : QObject(parent)
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");

    QString pathToDB = QString("C:/Users/Makarov/Desktop/mdb.sqlite");
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
     qDebug() << "Add";
     QSqlQuery query(myDB);
     query.prepare("INSERT INTO Users (UserName, Sex, Age, City, Password) VALUES (:UserName, :Sex, :Age, :City, :Password)");
     query.bindValue(":UserName", UserName);
     query.bindValue(":Sex", Sex);
     query.bindValue(":Age", Age);
     query.bindValue(":City", City);
     query.bindValue(":Password", Pas);
     query.exec();
}

bool SQLiteDB::FindInDB(QString UserName)
{
    qDebug() << "FindDB" << UserName;

    QSqlQuery query(myDB);
//    qDebug()<<query.prepare("SELECT * FROM Users WHERE UserName LIKE '%56%'");

//    if (query.prepare("SELECT UserName FROM Users WHERE UserName LIKE '%" +UserName+"%'"))
//    {
//        qDebug() << "Логин занят";
//        return true;
//    }

    query.prepare("SELECT * FROM qwerty WHERE UserName LIKE '%" + QString(UserName) + "%'");
    if(query.next())
            {
        qDebug() << "Логин занят";
                query.previous();
                 query.exec();
                return true;
            }

    query.exec();
    return false;
}
