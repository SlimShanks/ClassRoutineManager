#include "RoutineService.h"
#include <QSqlQuery>

namespace crm::services {

RoutineService::RoutineService(crm::database::Idatabase* db) : m_db(db) {}

// ── routines ──────────────────────────────────────────────────────────────────
QList<Routine> RoutineService::getAllRoutines()
{
    QSqlQuery q = m_db->query(
        "SELECT id, program, section, semester, year "
        "FROM routines ORDER BY year DESC, semester DESC"
    );

    QList<Routine> list;
    while (q.next()) {
        Routine r;
        r.id       = q.value(0).toInt();
        r.program  = q.value(1).toString();
        r.section  = q.value(2).toString();
        r.semester = q.value(3).toInt();
        r.year     = q.value(4).toInt();
        list.append(r);
    }
    return list;
}

std::optional<Routine> RoutineService::getRoutine(int id)
{
    QSqlQuery q = m_db->query(
        "SELECT id, program, section, semester, year "
        "FROM routines WHERE id = ?",
        { id }
    );
    if (!q.next()) return std::nullopt;

    Routine r;
    r.id       = q.value(0).toInt();
    r.program  = q.value(1).toString();
    r.section  = q.value(2).toString();
    r.semester = q.value(3).toInt();
    r.year     = q.value(4).toInt();
    return r;
}

int RoutineService::createRoutine(const Routine& r)
{
    bool ok = m_db->execute(
        "INSERT INTO routines (program, section, semester, year) "
        "VALUES (?, ?, ?, ?)",
        { r.program, r.section, r.semester, r.year }
    );
    if (!ok) return -1;
    return m_db->lastInsertId();
}

bool RoutineService::deleteRoutine(int id)
{
    // delete slots first to respect FK
    m_db->execute("DELETE FROM routine_slots WHERE routine_id = ?", { id });
    return m_db->execute("DELETE FROM routines WHERE id = ?", { id });
}

// ── slots ─────────────────────────────────────────────────────────────────────
QList<RoutineSlot> RoutineService::getSlotsForRoutine(int routineId)
{
    QSqlQuery q = m_db->query(
        "SELECT rs.id, rs.routine_id, rs.teacher_id, "
        "       t.name, t.subject, "
        "       rs.day, rs.start_time, rs.end_time "
        "FROM routine_slots rs "
        "JOIN teachers t ON t.id = rs.teacher_id "
        "WHERE rs.routine_id = ?",
        { routineId }
    );

    QList<RoutineSlot> list;
    while (q.next()) {
        RoutineSlot s;
        s.id          = q.value(0).toInt();
        s.routineId   = q.value(1).toInt();
        s.teacherId   = q.value(2).toInt();
        s.teacherName = q.value(3).toString();
        s.subject     = q.value(4).toString();
        s.day         = q.value(5).toString();
        s.startTime   = QTime::fromString(q.value(6).toString(), "HH:mm:ss");
        s.endTime     = QTime::fromString(q.value(7).toString(), "HH:mm:ss");
        list.append(s);
    }
    return list;
}

bool RoutineService::addSlot(const RoutineSlot& slot)
{
    return m_db->execute(
        "INSERT INTO routine_slots "
        "(routine_id, teacher_id, day, start_time, end_time) "
        "VALUES (?, ?, ?, ?, ?)",
        { slot.routineId, slot.teacherId, slot.day,
          slot.startTime.toString("HH:mm"),
          slot.endTime.toString("HH:mm") }
    );
}

bool RoutineService::updateSlot(const RoutineSlot& slot)
{
    return m_db->execute(
        "UPDATE routine_slots SET day=?, start_time=?, end_time=? WHERE id=?",
        { slot.day,
          slot.startTime.toString("HH:mm"),
          slot.endTime.toString("HH:mm"),
          slot.id }
    );
}

bool RoutineService::deleteSlot(int slotId)
{
    return m_db->execute(
        "DELETE FROM routine_slots WHERE id = ?", { slotId }
    );
}

} // namespace crm::services