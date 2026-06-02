#include "dbmanager.h"

#include <QSqlError>  // for getting error messages
#include <QDebug>     // for printing debug messages to console

DBManager::DBManager(const QString& dbPath)
{
    // add a SQLite database connection
    // "QSQLITE" is Qt's SQLite driver name
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    // tell it where the .db file is
    // if the file doesn't exist, SQLite will create it automatically
    m_db.setDatabaseName(dbPath);

    // try to open it
    if (!m_db.open()) {
        // if it fails, print the error to console
        qDebug() << "Database failed to open:" << m_db.lastError().text();
    } else {
        qDebug() << "Database opened successfully!";
        createTables();  // create tables on first run
    }
}

void DBManager::seedData()
{
    QSqlQuery query(m_db);

    // check if admin already exists
    // we don't want to insert it every time the app starts
    query.exec("SELECT * FROM users WHERE username = 'admin'");

    if (!query.next()) {
        // no admin found, insert one
        query.exec(
            "INSERT INTO users (username, password, role) "
            "VALUES ('admin', 'admin123', 'admin')"
        );
        qDebug() << "Default admin created!";
    } else {
        qDebug() << "Admin already exists, skipping seed.";
    }
}

bool DBManager::isOpen() const
{
    return m_db.isOpen();  // returns true if connection is open
}

void DBManager::createTables()
{
    QSqlQuery query(m_db);

    // ── Table 1: users ───────────────────────────────────────
    // stores login credentials for everyone
    query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"  // no two users can have same username
        "password TEXT NOT NULL,"
        "role     TEXT NOT NULL"          // 'admin' or 'teacher'
        ")"
    );

    // ── Table 2: teachers ────────────────────────────────────
    // stores teacher info, linked to users table
    query.exec(
        "CREATE TABLE IF NOT EXISTS teachers ("
        "id      INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"       // links to users.id
        "name    TEXT NOT NULL"
        ")"
    );

    // ── Table 3: courses ─────────────────────────────────────
    // stores what courses a teacher teaches
    query.exec(
        "CREATE TABLE IF NOT EXISTS courses ("
        "id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "teacher_id  INTEGER NOT NULL,"   // links to teachers.id
        "code        TEXT NOT NULL,"      // 'CS101'
        "course_name TEXT NOT NULL"
        ")"
    );

    // ── Table 4: routine_slots ───────────────────────────────
    // the actual timetable
    query.exec(
        "CREATE TABLE IF NOT EXISTS routine_slots ("
        "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "teacher_id INTEGER NOT NULL,"    // links to teachers.id
        "course_id  INTEGER NOT NULL,"    // links to courses.id
        "day        TEXT NOT NULL,"       // 'Monday' etc
        "start_time TEXT NOT NULL,"       // '09:00'
        "end_time   TEXT NOT NULL,"       // '11:00'
        "room       TEXT,"                // 'Room 101' - optional
        "section    TEXT NOT NULL"        // 'CSE-A', 'CSE-B' etc
        ")"
    );

    qDebug() << "Tables created successfully!";
}

bool DBManager::checkLogin(const QString& username, const QString& password)
{
    // create a query object that runs on our database
    QSqlQuery query(m_db);

    // prepare the SQL statement
    // :username and :password are placeholders — safer than putting
    // values directly in the string (prevents SQL injection attacks)
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");

    // replace the placeholders with actual values
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    // run the query
    if (query.exec()) {
        // query.next() moves to the first result row
        // if it returns true, a matching row was found
        if (query.next()) {
            return true;   // login success
        }
    } else {
        // something went wrong with the query itself
        qDebug() << "Login query failed:" << query.lastError().text();
    }

    return false;  // no match found
}

