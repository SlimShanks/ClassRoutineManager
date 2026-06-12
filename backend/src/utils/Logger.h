#pragma once
#include <QString>

namespace crm::utils { // utillity helper
// How seriously the systemwant to says thats log message from least to most serious).
enum class LogLevel { Debug, Info, Warning, Error };

// logging helper. Everything's static soi called it directly as->
//  Logger::info() without create of logger object
class Logger {
  public:
    // every logging shortcut below calls into this.
    static void log(LogLevel level, const QString &message);

    static void debug(const QString &m) { log(LogLevel::Debug, m); }     // shortcut for Debug
    static void info(const QString &m) { log(LogLevel::Info, m); }       // shortcut for Info
    static void warning(const QString &m) { log(LogLevel::Warning, m); } // shortcut for Warning
    static void error(const QString &m) { log(LogLevel::Error, m); }     // shortcut for Error
};

} // namespace crm::utils