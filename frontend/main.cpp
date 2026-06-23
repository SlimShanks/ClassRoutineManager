#include <QApplication>
#include "MainWindow.h"
#include "../backend/src/appcontext.h"
#include "../backend/src/Config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ---------------- CONFIG ----------------
    crm::Config config;
    config.host = "127.0.0.1";
    config.port = 3306;
    config.user = "root";
    config.password = "";
    config.database = "crm";

    // ---------------- BACKEND ----------------
    crm::AppContext ctx(config);

    if (!ctx.init()) {
        qDebug() << "Backend failed to initialize";
        return -1;
    }

    // ---------------- FRONTEND ----------------
    MainWindow window(&ctx);
    window.show();

    return app.exec();
}