

#include "MysqlDB.h" // our own declaration (always first)

#include <QSqlError> // lets the error message speak for itself
#include <QSqlQuery> // for running queries
#include <QVariant>  // QVariantList bindings
#include <utility>   // std::move -- avoids copying strings

#include "../utils/Logger.h" // structured logging

using crm::utils::Logger; // so we can write Logger::error(...) directly

namespace crm::database {

// Constructor: just store the settings. The network connection happens in open()
// because constructors cannot easily report failure.
Mysqldb::Mysqldb(Config config)
    : m_config(std::move(config)) { // move the settings in -- no string copies
    static int s_counter = 0;
    m_connName = QString("crm_mysql_%1").arg(++s_counter); // unique name per instance
}

// Destructor: make sure we disconnect when this object is destroyed.
Mysqldb::~Mysqldb() { close(); }

// open(): establish the network connection to the MySQL server.
bool Mysqldb::open() {
    m_db = QSqlDatabase::addDatabase("QMYSQL", m_connName); // pick MySQL driver
    m_db.setHostName(m_config.host);                        // where the server lives
    m_db.setPort(m_config.port);                            // which port
    m_db.setUserName(m_config.user);                        // login user
    m_db.setPassword(m_config.password);                    // login password
    m_db.setDatabaseName(m_config.database);                // which schema/database

    if (!m_db.open()) {
        Logger::error("Mysqldb open failed: " + m_db.lastError().text());
        return false; // report failure
    }
    Logger::info("Mysqldb: connected to " + m_config.host);
    return true;
}

// close(): disconnect, but only if we are currently connected.
void Mysqldb::close() {
    if (m_db.isOpen())
        m_db.close(); // avoid double-close
}

// isOpen(): true while a connection is live.
bool Mysqldb::isOpen() const { return m_db.isOpen(); }

/*
void DBManager::createTables() {
    QSqlQuery query(m_db);

    // ── Table 1: users ───────────────────────────────────────
    // stores login credentials for everyone
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id       INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT NOT NULL UNIQUE," // no two users can have same username
               "password TEXT NOT NULL,"
               "role     TEXT NOT NULL" // 'admin' or 'teacher'
               ")");

    // ── Table 2: teachers ────────────────────────────────────
    // stores teacher info, linked to users table
    query.exec("CREATE TABLE IF NOT EXISTS teachers ("
               "id      INTEGER PRIMARY KEY AUTOINCREMENT,"
               "user_id INTEGER NOT NULL," // links to users.id
               "name    TEXT NOT NULL"
               ")");

    // ── Table 3: courses ─────────────────────────────────────
    // stores what courses a teacher teaches
    query.exec("
               ")");

    // ── Table 4: routine_slots ───────────────────────────────
    // the actual timetable
    query.exec("CREATE TABLE IF NOT EXISTS routine_slots ("
               "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
               "teacher_id INTEGER NOT NULL," // links to teachers.id
               "course_id  INTEGER NOT NULL," // links to courses.id
               "day        TEXT NOT NULL,"    // 'Monday' etc
               "start_time TEXT NOT NULL,"    // '09:00'
               "end_time   TEXT NOT NULL,"    // '11:00'
               "room       TEXT,"             // 'Room 101' - optional
               "section    TEXT NOT NULL"     // 'CSE-A', 'CSE-B' etc
               ")");

    qDebug() << "Tables created successfully!";
}
*/

// migrate(): create the application tables if they do not exist yet.
// Returns true on success, false if any statement fails.
bool Mysqldb::migrate() {
    // Users table -------------------------------------------------------
    if (!execute("CREATE TABLE IF NOT EXISTS users ("
                 "  id            INT AUTO_INCREMENT PRIMARY KEY,"
                 "  server_id     INT,"                        // remote id (for SyncManager)
                 "  dirty         TINYINT NOT NULL DEFAULT 1," // 1 = needs push
                 "  deleted       TINYINT NOT NULL DEFAULT 0," // soft-delete flag
                 "  updated_at    DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE "
                 "CURRENT_TIMESTAMP,"
                 "  name          VARCHAR(120) NOT NULL,"
                 "  email         VARCHAR(255) NOT NULL UNIQUE,"
                 "  role          VARCHAR(40)  NOT NULL DEFAULT 'viewer',"
                 "  password_hash VARCHAR(255) NOT NULL"
                 ")"))
        return false;

    // Resources table (rooms, teachers, projectors …) -------------------
    if (!execute(
            "CREATE TABLE IF NOT EXISTS resources ("
            "  id         INT AUTO_INCREMENT PRIMARY KEY,"
            "  server_id  INT,"
            "  dirty      TINYINT NOT NULL DEFAULT 1,"
            "  deleted    TINYINT NOT NULL DEFAULT 0,"
            "  updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
            "  name       VARCHAR(120) NOT NULL,"
            ")"))
        return false;

    // Timetables table --------------------------------------------------
    if (!execute(
            "CREATE TABLE IF NOT EXISTS timetables ("
            "  id         INT AUTO_INCREMENT PRIMARY KEY,"
            "  server_id  INT,"
            "  dirty      TINYINT NOT NULL DEFAULT 1,"
            "  deleted    TINYINT NOT NULL DEFAULT 0,"
            "  updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
            "  name       VARCHAR(120) NOT NULL"
            ")"))
        return false;

    // Schedules table ---------------------------------------------------
    if (!execute("CREATE TABLE IF NOT EXISTS schedules ("
                 "  id           INT AUTO_INCREMENT PRIMARY KEY,"
                 "  server_id    INT,"
                 "  dirty        TINYINT NOT NULL DEFAULT 1,"
                 "  deleted      TINYINT NOT NULL DEFAULT 0,"
                 "  updated_at   DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE "
                 "CURRENT_TIMESTAMP,"
                 "  timetable_id INT NOT NULL,"
                 "  resource_id  INT NOT NULL,"
                 "  day_of_week  VARCHAR(10) NOT NULL,"
                 "  start_time   TIME        NOT NULL,"
                 "  end_time     TIME        NOT NULL,"
                 "  subject      VARCHAR(120) NOT NULL"
                 ")"))
        return false;

    Logger::info("Mysqldb: migrate() complete");
    return true;
}

// execute(): run an INSERT / UPDATE / DELETE statement with bound parameters.
bool Mysqldb::execute(const QString &sql, const QVariantList &params) {
    QSqlQuery q = prepare(sql, params);

    if (!q.exec()) {
        Logger::error("Mysqldb execute failed: " + q.lastError().text());
        return false;
    }
    return true;
}

// query(): run a SELECT and return the result so the caller can iterate rows.
QSqlQuery Mysqldb::query(const QString &sql, const QVariantList &params) {
    QSqlQuery q = prepare(sql, params);

    if (!q.exec())
        Logger::error("Mysqldb query failed: " + q.lastError().text());
    return q; // the caller calls q.next() to walk the rows
}

// returns the auto-increment id the server assigned for the last INSERT.
int Mysqldb::lastInsertId() const {
    QSqlQuery q(m_db);

    q.exec("SELECT LAST_INSERT_ID()");
    if (q.next())
        return q.value(0).toInt();
    return -1; // should never happen after a successful INSERT
}

// prepare(): bind parameters safely so the driver handles escaping (no SQL injection).
QSqlQuery Mysqldb::prepare(const QString &sql, const QVariantList &params) {
    QSqlQuery q(m_db);
    q.prepare(sql);
    for (int i = 0; i < params.size(); ++i)
        q.addBindValue(params.at(i)); // positional binding for each "?"
    return q;
}

} // namespace crm::database
