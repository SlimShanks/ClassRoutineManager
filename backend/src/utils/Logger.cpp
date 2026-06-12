#include "Logger.h"  // our own declaration
#include <QDateTime> // to timestamp
#include <QDebug>    // qInfo / qCritical ko lagi

namespace crm::utils { // utility helpers live here

static QString levelName(LogLevel level) {
    switch (level) { // choose text based on the level
    case LogLevel::Debug:
        return "DEBUG"; // debug messages
    case LogLevel::Info:
        return "INFO"; // normal infor
    case LogLevel::Warning:
        return "WARN"; // something wrong
    case LogLevel::Error:
        return "ERROR"; // something failed
    }
    return "?"; // fallback (should never be reached)
}

// building and printin one log line at a time
void Logger::log(LogLevel level, const QString &message) {
    const QString ts = QDateTime::currentDateTime().toString(Qt::ISODate); // current time as text
    const QString line =
        QString("[%1] %2  %3").arg(ts, levelName(level), message); // fill %1 %2 %3 in order
    if (level == LogLevel::Error)                                  // error more so serious
        qCritical().noquote() << line; // send them to the critical messg
    else                               // everything else
        qInfo().noquote() << line;     // goes to normal output i.e.noquote = no extra quotes)
} // end Logger::log

} // namespace crm::utils