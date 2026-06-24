#pragma once
#include "database/IDatabase.h"
#include "TeacherService.h"
#include <QString>
#include <QList>
#include <QTime>
#include <optional>

namespace crm::services {

struct Routine {
    int     id       = -1;
    QString program;
    QString section;
    int     semester = 1;
    int     year     = 2025;
};

struct RoutineSlot {
    int     id        = -1;
    int     routineId = -1;
    int     teacherId = -1;
    QString teacherName;   // joined from teachers table
    QString subject;       // joined from teachers table
    QString day;
    QTime   startTime;
    QTime   endTime;
};

class RoutineService {
public:
    explicit RoutineService(crm::database::Idatabase* db);

    // routines
    QList<Routine>       getAllRoutines();
    std::optional<Routine> getRoutine(int id);
    int                  createRoutine(const Routine& r);  // returns new id, -1 on fail
    bool                 deleteRoutine(int id);

    // slots
    QList<RoutineSlot>   getSlotsForRoutine(int routineId);
    bool                 addSlot(const RoutineSlot& slot);
    bool                 updateSlot(const RoutineSlot& slot);  // resize
    bool                 deleteSlot(int slotId);

private:
    crm::database::Idatabase* m_db;
};

} // namespace crm::services