// main.cpp — the program's "composition root": the ONE place that builds the
// settings, constructs the backend (via AppContext), and hands it to the
// frontend. Nothing else needs to know how the pieces fit together.

#include <QCoreApplication> // Qt runtime; needed for the SQL drivers.
                            // (Becomes <QApplication> once you add the GUI.)
#include "Config.h"         // the hardcoded settings bag
#include "appcontext.h"     // the frontend<->backend wiring hub
#include "utils/Logger.h"   // logging
// When you add the GUI, also: #include "frontend/MainWindow.h"

using namespace crm; // so we can drop the "crm::" prefix below

int main(int argc, char **argv) {     // program starts here
    QCoreApplication app(argc, argv); // set up Qt's runtime
                                      // (use QApplication app(...) for the GUI)

    // ---hardcoded settings Config is just the defination and this is just the values ---
    Config config;             // create settings object
    config.host = "127.0.0.1"; // MySQL server address
    config.port = 3306;        // default MySQL port already setup in config
    config.user = "root";      //  login user
    config.password = "";      //  password
    config.database = "crm";   // which schema to use

    // --build/bundle + wire the ENTIRE backend in two lines. ---
    AppContext ctx(config); // remember the settings
    if (!ctx.init()) {      // open MySQL, migrate, build repos+services
        crm::utils::Logger::error("Startup failed: backend not ready."); // connection/setup problem
        return 1; // non-zero = the program failed
    }
}