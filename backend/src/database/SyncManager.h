#pragma once

#include "IDatabase.h" // the interface both databases implement
#include "MysqlDB.h"   // the REMOTE database (MySQL/)
#include "SQLiteDB.h"  // the LOCAL database (SQLite)
#include <QString>
#include <QStringList>
#include <QVector>
#include <memory> // std::unique_ptr

namespace crm::database {

// Describes ONE table to keep in sync, listing its real DATA columns only.
// (id + sync bookkeeping columns -- server_id, dirty, deleted, updated_at -- are automatic.)
struct SyncTable {
    QString name;            // e.g. "users"
    QStringList dataColumns; // e.g. {"name", "email", "role", "password_hash"}
}; // end struct SyncTable

// SyncManager is the SINGLE data-layer entry point.
//
// It owns:
//   * a LOCAL database (SQLite) -- always available, fast, works offline.
//   * a REMOTE database (MySQL) -- the shared server, optional.
//
// Design: PUSH-ONLY (local -> server). The admin is the only writer; viewer
// machines read MySQL directly (read-only). There is no pull() and no conflict
// resolution. If a second writer is ever added, re-introduce pull().
class SyncManager {
  public:
    // PRODUCTION: build from a SQLite file path + MySQL connection settings.
    SyncManager(QString localDbPath, Config remoteConfig);

    // TESTING / advanced: inject any two IDatabase implementations.
    // (e.g. two in-memory SQLite instances so unit tests need no network.)
    SyncManager(std::unique_ptr<Idatabase> local, std::unique_ptr<Idatabase> remote,
                QVector<SyncTable> tables);

    // Declared here (not inline) so unique_ptr<Idatabase> can be destroyed where
    // the full concrete types are known (i.e. in SyncManager.cpp).
    ~SyncManager();

    bool start(); // open + migrate the local DB (required); try the server (optional)

    Idatabase *local();  // the DB that repositories should read/write
    Idatabase *remote(); // the server connection (mainly for diagnostics / tests)

    bool isOnline(); // true if the server is reachable right now

    int push();  // upload every table's dirty rows; returns the number of rows pushed
    bool sync(); // run one upload cycle; returns true if the push ran

  private:
    int pushTable(const SyncTable &t); // upload one table's dirty rows

    static QVector<SyncTable> defaultTables(); // the app's standard table list

    std::unique_ptr<Idatabase> m_local;  // owns the local SQLite cache
    std::unique_ptr<Idatabase> m_remote; // owns the MySQL server connection
    QVector<SyncTable> m_tables;         // which tables to keep in sync
};

} // namespace crm::database