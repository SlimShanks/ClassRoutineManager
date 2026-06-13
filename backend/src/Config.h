// tldr; this is for the configuration file

#pragma once       // include this header only once per compilation
#include <QString> // Qt's text string type

namespace crm { // top-level project namespace

// A plain settings bag: create one, fill in the fields, pass it around.
struct Config {
    QString sqlitePath =
        "data/crm.db"; // path to the SQLite file also for now i have not made the file if some one
                       // is reading and not created then create it

    QString host;     // server
    int port = 3306;  // Mysql host port
    QString user;     // db username
    QString password; // db password
    QString database; // Mysql database nam

    bool syncEnabled() const { return !host.isEmpty(); } // sync only if a host was set
}; // for up this now end of configuration is in need
} // namespace crm
