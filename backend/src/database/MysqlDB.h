#pragma once

#include "IDatabase.h"  // the interface all database drivers must implement
#include <QSqlDatabase> // Qt's database connection handler
#include <QSqlQuery>    // for query results
#include <QString>      // Qt's string type
#include <QVariantList> // for parameter lists

namespace crm::database {

// Connection settings for the MySQL/ server.
// All fields are values (not references) so the struct can be safely stored and moved.
struct Config {
    QString host;     // hostname or IP of the MySQL server
    int port = 3306;  // port (default MySQL port)
    QString user;     // login username
    QString password; // login password
    QString database; // which schema/database to select

    bool syncEnabled() const { return !host.isEmpty(); } // sync only if a host was set
}; // end struct Config

// Mysqldb talks to the remote MySQL/ server.
// It implements the IDatabase interface so the rest of the app never depends on
// MySQL directly -- swapping to another engine means only changing AppContext.
class Mysqldb : public Idatabase {
  public:
    explicit Mysqldb(Config config); // store connection settings (does NOT connect yet)
    ~Mysqldb() override;             // closes the connection when destroyed

    bool open() override;         // connect to the MySQL server
    void close() override;        // disconnect
    bool isOpen() const override; // true while a connection is live
    bool migrate() override;      // create tables if they do not exist yet

    bool execute(const QString &sql,
                 const QVariantList &params = {}) override; // INSERT / UPDATE / DELETE
    QSqlQuery query(const QString &sql,
                    const QVariantList &params = {}) override; // SELECT -- returns result rows

    int lastInsertId() const override; // id assigned by the server for the last INSERT

  private:
    QSqlQuery prepare(const QString &sql,
                      const QVariantList &params = {}); // bind params safely (no SQL injection)

    Config m_config;    // connection settings given at construction time
    QString m_connName; // unique Qt connection name for this instance
    QSqlDatabase m_db;  // the live connection handle
}; // end class Mysqldb

} // namespace crm::database
