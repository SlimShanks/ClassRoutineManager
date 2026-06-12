#include "appcontext.h" // our own declaration (always include it first)

#include <utility> // std::move (for not using the copy string)

#include "database/MysqlDB.h" // the CONCRETE database thats builded
#include "utils/Logger.h"     // logging

namespace crm { // top-level project namespace

// Constructor: just store the settings. We connectlater in init(),
// because connecting can fail and a constructor cannot easily report failure.
AppContext::AppContext(Config config)
    : m_config(std::move(config)) {} // move the settings into our member

// Destructor: empty body, but it matters. The unique_ptr members are destroyed
// in REVERSE order (db at last as the auth not defined and other services also),
//  so the service/repo die before the
// database they borrow.
AppContext::~AppContext() = default;

// init(): build and connect the whole backend, from  bottom-up approach
bool AppContext::init() {
    // 1) Translate the app-wide Config into the DB-specific Config.
    //    (Config is the single hardcoded source; Config is what MysqlDb wants.)
    database::Config mc;             // start with MySQL's settings struct
    mc.host = m_config.host;         // copy each field across
    mc.port = m_config.port;         //
    mc.user = m_config.user;         //
    mc.password = m_config.password; //
    mc.database = m_config.database; //

    // 2) Build the concrete database, but keep it behind the IDatabase interface.
    //    Everything above the DB only sees IDatabase, so swapping engines is easy.
    m_db = std::make_unique<database::Mysqldb>(mc);

    // 3) Open the network connection to the server.
    if (!m_db->open()) { // false = could not connect
        utils::Logger::error("AppContext: could not open MySQL connection.");
        return false; // stop: backend not ready
    }

    // 4) Create the tables if they do not exist yet.
    if (!m_db->migrate()) { // false = table setup failed
        utils::Logger::error("AppContext: migrate() failed.");
        return false; // stop: backend not ready
    }

    m_ready = true; // every layer wired up
    utils::Logger::info("AppContext: backend ready (MySQL).");
    return true; // success
}

// Has the backend been built successfully?
bool AppContext::isReady() const { return m_ready; }

} // namespace crm