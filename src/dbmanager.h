#pragma once

#include <QSqlQuery> // handles the database queries
#include <QSqlError> // used to get error messages from the database
#include <QString> // QT's string type
#include <QSqlDatabase> // database connection type

class DBManager{
    public:
    DBManager(const QString& dbpath); //constructor that takes the path to the database file
    bool isOpen() const; // checks if the database connection is open
    bool checkLogin(const QString& username, const QString& password); // auth
    void createTables(); // creates all tabls if the dont exist 
    void seedData(); // insets default admin if user if not already there

    private:
    QSqlDatabase m_db; // the database connection   

};