#include "ConflictService.h"
#include <QSqlQuery>

namespace crm::services {

ConflictService::ConflictService(crm::database::Idatabase* db) : m_db(db) {}

QList<ConflictInfo> ConflictService::checkConflict(int teacherId,
                                                     const QString& day,
                                                     const QTime& startTime,
                                                     const QTime& endTime,
                                                     int excludeSlotId)
{
    // Overlap rule: two ranges [aStart, aEnd) and [bStart, bEnd) overlap iff
    //   aStart < bEnd AND aEnd > bStart
    QSqlQuery q = m_db->query(
        "SELECT rs.id, rs.routine_id, r.program, r.section, "
        "       rs.day, rs.start_time, rs.end_time "
        "FROM routine_slots rs "
        "JOIN routines r ON r.id = rs.routine_id "
        "WHERE rs.teacher_id = ? "
        "  AND rs.day = ? "
        "  AND rs.id != ? "
        "  AND rs.start_time < ? "
        "  AND rs.end_time > ?",
        { teacherId, day, excludeSlotId,
          endTime.toString("HH:mm"),
          startTime.toString("HH:mm") }
    );

    QList<ConflictInfo> conflicts;
    while (q.next()) {
        ConflictInfo c;
        c.slotId    = q.value(0).toInt();
        c.routineId = q.value(1).toInt();
        c.program   = q.value(2).toString();
        c.section   = q.value(3).toString();
        c.day       = q.value(4).toString();
        c.startTime = QTime::fromString(q.value(5).toString(), "HH:mm");
        c.endTime   = QTime::fromString(q.value(6).toString(), "HH:mm");
        conflicts.append(c);
    }
    return conflicts;
}

bool ConflictService::hasConflict(int teacherId,
                                   const QString& day,
                                   const QTime& startTime,
                                   const QTime& endTime,
                                   int excludeSlotId)
{
    return !checkConflict(teacherId, day, startTime, endTime, excludeSlotId).isEmpty();
}

} // namespace crm::services