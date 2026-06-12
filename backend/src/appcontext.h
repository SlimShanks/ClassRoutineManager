#pragma once // include this header only once per compilation

#include <memory> // std::unique_ptr (owns objects, frees them automatically)

#include "Config.h"                // the hardcoded settings bag
#include "database/IDatabase.h"    // the database INTERFACE (swap feature /SQLite)
#include "repositories/UserRepo.h" // user storage layer
#include "services/AuthService.h"  // account logic layer

namespace crm { // top-levelproject namespace

// appcontext meana bridge between the frontend and the backend.
// UI never builds a database it just holds ONE
// appcontext and asks it for ready-made services
// internally it owns the whole thing
class AppContext {
  public:
    explicit AppContext(Config config); // rememberin hardcoded settings; does NOT connect yet
    ~AppContext();                      // closes the database connection cleanly and neat

    // build everything open the DB, create tables, ,igrate,false if any step fails
    bool init();

    bool isReady() const; // true once init() has succeeded
    // for auth services declaration to be left and also could be done here the auth should be
    // defined

  private:
    Config m_config;                           // hardcoded setting
    std::unique_ptr<database::Idatabase> m_db; // owns the live DB connection
    bool m_ready = false;                      // for 1 or 0 init() finished or not
}; // end class AppContext

} // namespace crm