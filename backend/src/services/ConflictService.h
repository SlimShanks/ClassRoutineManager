#pragma once
#include "database/IDatabase.h"
#include <QString>
#include <QList>
#include <QTime>

namespace crm::services {

struct ConflictInfo {
    int     slotId      = -1;
    int     routineId   = -1;
    QString program;
    QString section;
    QString day;
    QTime   startTime;
    QTime   endTime;
};

class ConflictService {
public:
    explicit ConflictService(crm::database::Idatabase* db);

    // Returns all existing slots (across ALL routines) where the SAME teacher
    // is already booked on `day` with a time range overlapping [startTime, endTime).
    // Pass excludeSlotId when editing an existing slot so it doesn't conflict with itself.
    QList<ConflictInfo> checkConflict(int teacherId,
                                       const QString& day,
                                       const QTime& startTime,
                                       const QTime& endTime,
                                       int excludeSlotId = -1);

    // Convenience wrapper
    bool hasConflict(int teacherId,
                      const QString& day,
                      const QTime& startTime,
                      const QTime& endTime,
                      int excludeSlotId = -1);

private:
    crm::database::Idatabase* m_db;
};

} // namespace crm::services