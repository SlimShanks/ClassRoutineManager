// SQLitedb.cpp — the only part of the program that talks to the SQLite file.
// Everything above it (repositories, services) just asks it to run SQL.

#include "SQLiteDB.h"
#include "../utils/Logger.h" // for logging errors and info

#include <QSqlError> // lets us read WHY a query failed
#include <utility>   // gives us std::move

using crm::utils::Logger; // so we can write Logger::info(...)

namespace crm::database { // database code lives here

// Constructor: just store the file path. std::move avoids copying the string.
SQLitedb::SQLitedb(QString dbpath) : m_dbpath(std::move(dbpath)) {
    static int s_counter = 0; // one shared counter across ALL SQLitedb objects

    m_connName =
        QString("crm_sqlite_%1").arg(++s_counter); // give THIS instance a unique connection name
}

// Destructor: for disconnect when object destroyed.
SQLitedb::~SQLitedb() { close(); }

// open(): connect to (or create) the SQLite file on disk.
bool SQLitedb::open() {
    m_db = QSqlDatabase::addDatabase(
        "QSQLITE", m_connName); // pick SQLite driver + use this instance's unique name

    m_db.setDatabaseName(m_dbpath); // point it at our file (created if missing)

    if (!m_db.open()) {                                              // try to open; if it fails...
        Logger::error("SQLite open failed: " + m_db.lastError().text()); // log the reason
        return false;                                                // and report failure
    }

    Logger::info("SQLite opened at " + m_dbpath); // note success
    return true;                                  // report success
}

// close(): disconnect, but only if we are currently open.
void SQLitedb::close() {
    if (m_db.isOpen())
        m_db.close(); // avoid closing an already-closed connection
}

// isOpen(): true while a connection is live.
bool SQLitedb::isOpen() const { return m_db.isOpen(); }

// migrate(): create every table the app needs (safe to run repeatedly).
bool SQLitedb::migrate() {
    // Every syncable table carries four extra "sync metadata" columns so the
    // SyncManager knows what changed locally and how to reconcile with Mysql:
    //   updated_at -> ISO-8601 time of the last local change (used for conflicts)
    //   dirty      -> 1 = changed locally and NOT yet pushed to the server
    //   deleted    -> 1 = "tombstone": row is gone but kept so the delete can sync
    //   server_id  -> this row's id on Mysql (the local id and server id differ)
    static const char *schema = R"SQL(
        CREATE TABLE IF NOT EXISTS users (
            id            INTEGER PRIMARY KEY AUTOINCREMENT, -- unique id, auto-filled
            name          TEXT NOT NULL,
            email         TEXT NOT NULL UNIQUE,              -- no two users share an email
            --just the assumption for admin 0 and also for the teacher similarly 1
            role          INTEGER NOT NULL DEFAULT 1,        -- matches Role enum (1 = teacher)
            password_hash TEXT NOT NULL,                     -- the hash, never the real password
            updated_at    TEXT NOT NULL DEFAULT '',          -- sync: time of last local change
            dirty         INTEGER NOT NULL DEFAULT 1,        -- sync: 1 = needs pushing to server
            deleted       INTEGER NOT NULL DEFAULT 0,        -- sync: 1 = tombstone (delete pending)
            server_id     INTEGER                            -- sync: matching row id on Mysql
        );
        ---same same parameter but different sql table
        CREATE TABLE IF NOT EXISTS resources (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            name       TEXT NOT NULL,
            updated_at TEXT NOT NULL DEFAULT '',             -- sync: time of last local change
            dirty      INTEGER NOT NULL DEFAULT 1,           -- sync: 1 = needs pushing to server
            deleted    INTEGER NOT NULL DEFAULT 0,           -- sync: 1 = tombstone (delete pending)
            server_id  INTEGER                               -- sync: matching row id on Mysql
        );
        CREATE TABLE IF NOT EXISTS timetables (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            name       TEXT NOT NULL,
            updated_at TEXT NOT NULL DEFAULT '',             -- sync: time of last local change
            dirty      INTEGER NOT NULL DEFAULT 1,           -- sync: 1 = needs pushing to server
            deleted    INTEGER NOT NULL DEFAULT 0,           -- sync: 1 = tombstone (delete pending)
            server_id  INTEGER                               -- sync: matching row id on Mysql
        );
        --this is just for test that for the schedules
        CREATE TABLE IF NOT EXISTS schedules (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            timetable_id INTEGER NOT NULL REFERENCES timetables(id) ON DELETE CASCADE, -- remove slots with their timetable
            resource_id  INTEGER NOT NULL REFERENCES resources(id),
            day_of_week  INTEGER NOT NULL,                   -- 1 = Monday ... 7 = Sunday
            start_time   TEXT NOT NULL,                      -- stored as text like "09:00"
            end_time     TEXT NOT NULL,
            subject      TEXT NOT NULL,
            updated_at   TEXT NOT NULL DEFAULT '',           -- sync: time of last local change
            dirty        INTEGER NOT NULL DEFAULT 1,         -- sync: 1 = needs pushing to server
            deleted      INTEGER NOT NULL DEFAULT 0,         -- sync: 1 = tombstone (delete pending)
            server_id    INTEGER                             -- sync: matching row id on Mysql
        );
        CREATE TABLE IF NOT EXISTS sync_state (
            table_name TEXT PRIMARY KEY,                     -- one row per syncable table
            last_pull  TEXT NOT NULL DEFAULT ''              -- ISO-8601 time we last pulled this table
        );
    )SQL"; // end of the SQL script

    for (const QString &stmt :
         QString(schema).split(';', Qt::SkipEmptyParts)) { // run one statement at a time
        const QString trimmed = stmt.trimmed();            // remove surrounding whitespace
        if (trimmed.isEmpty())
            continue;           // skip blank leftovers
        QSqlQuery q(m_db);      // a query bound to our connection
        if (!q.exec(trimmed)) { // run it; if it fails...
            Logger::error("Migration failed: " + q.lastError().text()); // ...log why
            return false;                                               // ...and stop
        }
    }
    Logger::info("Schema migration complete"); // all tables are ready
    return true;                               // success
}

// to-do: prepare() -- build a safe query using "?" placeholders (prevents SQL injection).
//
// what needs to happen here:
//   1. create a QSqlQuery on m_db
//   2. call q.prepare(sql) to load the SQL template
//   3. loop over params and call q.addBindValue(p) for each one
//   4. return the query ready to run
QSqlQuery SQLitedb::prepare(const QString &sql, const QVariantList &params) {
    QSqlQuery q(m_db);               // a query on our connection
    q.prepare(sql);                  // give it the SQL template containing "?" marks
    for (const QVariant &p : params) // for each value provided...
        q.addBindValue(p);           // ...fill the next "?" in order
    return q;                        // hand back the ready-to-run query
}

// execute(): run SQL that CHANGES data (INSERT/UPDATE/DELETE).
bool SQLitedb::execute(const QString &sql, const QVariantList &params) {
    QSqlQuery q = prepare(sql, params); // build the safe query

    // for log function only not much of thing
    if (!q.exec()) {                 // run it; if it fails...
        Logger::error("Execute failed: " + q.lastError().text() +
                      " | SQL: " + sql); // ...log details
        return false;                    // ...report failure
    }
    // and the insertion for to find out the auto generated id of the rows it just inserted
    m_lastInsertId = q.lastInsertId().toInt(); // remember the new row's id
    return true;                               // success
}

// query(): run SQL that READS data (SELECT)
QSqlQuery SQLitedb::query(const QString &sql, const QVariantList &params) {
    QSqlQuery q = prepare(sql, params); // build the safe query

    // for log function only not much of thing
    if (!q.exec()) // run it; if it fails...
        Logger::error("Query failed: " + q.lastError().text() + " | SQL: " + sql); // ...log details
    return q; // return the result either way
}

// lastInsertId(): the id saved by the most recent execute().
int SQLitedb::lastInsertId() const { return m_lastInsertId; }

} // namespace crm::database
