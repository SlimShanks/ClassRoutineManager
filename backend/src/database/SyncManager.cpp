

#include "SyncManager.h"     // our own declaration

#include <QSqlQuery>         // to step through query results
#include <QVariant>          // the "any-type" value used for cells/params
#include <utility>           // gives us std::move

#include "../utils/Logger.h" // logging status

using crm::utils::Logger; // so we can write Logger::info(...)

namespace crm::database { // database code lives here

// The app's standard list of synced tables, each with its real DATA columns only
// (id + sync bookkeeping columns are handled automatically).
QVector<SyncTable> SyncManager::defaultTables() {
    return {
        {"users",      {"name", "email", "role", "password_hash"}}, // user accounts
        {"resources",  {"name", "type"}},                           // rooms/teachers/etc
        {"timetables", {"name"}},                                    // named timetables
        {"schedules",  {"timetable_id", "resource_id", "day_of_week",
                        "start_time", "end_time", "subject"}},      // individual slots
    };
}

// PRODUCTION constructor: build the concrete local (SQLite) + remote (MySQL) DBs.
SyncManager::SyncManager(QString localDbPath, Config remoteConfig)
    : m_local(std::make_unique<SQLitedb>(std::move(localDbPath))),  // the local cache
      m_remote(std::make_unique<Mysqldb>(std::move(remoteConfig))), // the server
      m_tables(defaultTables()) {}                                   // sync the standard tables

// TESTING constructor: take ownership of two databases handed to us.
SyncManager::SyncManager(std::unique_ptr<Idatabase> local, std::unique_ptr<Idatabase> remote,
                         QVector<SyncTable> tables)
    : m_local(std::move(local)), m_remote(std::move(remote)), m_tables(std::move(tables)) {}

// Destructor: empty, but defined here so the unique_ptr<Idatabase> members can be
// destroyed where the full types are known.
SyncManager::~SyncManager() = default;

// start(): get the data layer ready. The LOCAL database MUST work; the server is
// optional (we stay fully usable offline -- that is the whole point).
bool SyncManager::start() {
    if (!m_local->open())    return false; // the local cache MUST open
    if (!m_local->migrate()) return false; // make sure the local tables exist

    if (m_remote->open()) {                 // try the server too...
        m_remote->migrate();                // ...and ensure its tables exist
        Logger::info("SyncManager: server online");
    } else {                                                             // server unreachable?
        Logger::info("SyncManager: server offline, running local-only"); // that's fine
    }
    return true; // ready as long as the local cache works
}

// local(): the database repositories should read from and write to.
Idatabase *SyncManager::local() { return m_local.get(); }

// remote(): the server connection (exposed mainly for tests / diagnostics).
Idatabase *SyncManager::remote() { return m_remote.get(); }

// isOnline(): is the server reachable right now?
bool SyncManager::isOnline() {
    if (m_remote->isOpen()) return true;  // already connected -> yes
    return m_remote->open();              // otherwise try to connect and report the result
}

int SyncManager::pushTable(const SyncTable &t) { Q_UNUSED(t) return 0; }

int SyncManager::push() { return 0; }

bool SyncManager::sync() { return false; }

} // namespace crm::database