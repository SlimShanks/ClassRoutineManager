
#pragma once

#include "IDatabase.h"  // the interface for database operatins we made in the IDatabase.h file
#include <QSqlDatabase> //QT's database connection handler
#include <qdebug.h>
#include <qglobal.h>
#include <qsqlquery.h>

namespace crm::database {

// SqliteDatabase.h implements the Idatabase.h for SQLite databases
// Every fuction the Idatabase declared. "override" proves we match each one : - )

class SQLitedb : public Idatabase {
  public:
    explicit SQLitedb(
        QString dbpath);  // remember which files to use where the dbpath mean of files to use
    ~SQLitedb() override; // closing the connection when destroyer destrys

    bool open() override;         // conncet to / also create the db file
    void close() override;        // disconnects
    bool isOpen() const override; // is it open?
    bool migrate() override;      // if not avaiable create the tables

    bool execute(const QString &sql, const QVariantList &params = {})
        override; // this same as in the Idatabase.h have just the implemetnion
    QSqlQuery query(const QString &sql,
                    const QVariantList &params = {}) override; // SELECT and return the results

    int lastInsertId() const override; // id assigned by SQLite for the last INSERT

  private:
    QSqlQuery prepare(const QString &sql,
                      const QVariantList &params = {}); // building the safequery

    QString m_dbpath;       // paths to .db file where m_ is for marking the memmber variable
    QString m_connName;     // unique Qt connection name for THIS instance (lets many DBs coexist)
    QSqlDatabase m_db;      // the open connection
    int m_lastInsertId = 0; // remembers the id of the last insert
};

} // namespace crm::database
