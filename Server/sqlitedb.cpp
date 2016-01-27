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
    qDebug() << "Поиск в БД: " << UserName;

    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName FROM Users WHERE UserName=\'" +UserName+ "\'"))
        if(query.next())
            if (query.value(0).toString()==UserName)
                return true;

    query.exec();
    return false;
}

bool SQLiteDB::CorrectInput(QString _login, QString _password)
{
    qDebug() << "Проверка Логина/Пароля";

    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Password FROM Users WHERE UserName=\'" +_login+ "\' AND Password=\'"+_password+ "\'"))
        if(query.next())
            if (query.value(0).toString()== _login && query.value(1).toString()==_password)
                return true;   

    query.exec();
    return false;
}
