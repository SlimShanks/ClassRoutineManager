#include "TeacherService.h"
#include <QSqlQuery>

namespace crm::services {

TeacherService::TeacherService(crm::database::Idatabase* db) : m_db(db) {}

std::optional<TeacherProfile> TeacherService::getProfile(int userId)
{
    QSqlQuery q = m_db->query(
        "SELECT id, user_id, name, subject, department, "
        "       days, start_time, end_time "
        "FROM teachers WHERE user_id = ? LIMIT 1",
        { userId }
    );

    if (!q.next()) return std::nullopt;

    TeacherProfile p;
    p.id         = q.value(0).toInt();
    p.userId     = q.value(1).toInt();
    p.name       = q.value(2).toString();
    p.subject    = q.value(3).toString();
    p.department = q.value(4).toString();
    p.days       = q.value(5).toString().split(',', Qt::SkipEmptyParts);
    p.startTime = QTime::fromString(q.value(6).toString(), "HH:mm:ss");
    p.endTime   = QTime::fromString(q.value(7).toString(), "HH:mm:ss");
    return p;
}

bool TeacherService::createProfile(const TeacherProfile& p)
{
    return m_db->execute(
        "INSERT INTO teachers "
        "(user_id, name, subject, department, days, start_time, end_time) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)",
        { p.userId, p.name, p.subject, p.department,
          p.days.join(','),
          p.startTime.toString("HH:mm"),
          p.endTime.toString("HH:mm") }
    );
}

bool TeacherService::updateProfile(const TeacherProfile& p)
{
    return m_db->execute(
        "UPDATE teachers SET name=?, subject=?, department=?, "
        "days=?, start_time=?, end_time=? WHERE id=?",
        { p.name, p.subject, p.department,
          p.days.join(','),
          p.startTime.toString("HH:mm"),
          p.endTime.toString("HH:mm"),
          p.id }
    );
}

QList<TeacherProfile> TeacherService::getAllProfiles()
{
    QSqlQuery q = m_db->query(
        "SELECT id, user_id, name, subject, department, "
        "       days, start_time, end_time "
        "FROM teachers ORDER BY name"
    );

    QList<TeacherProfile> list;
    while (q.next()) {
        TeacherProfile p;
        p.id         = q.value(0).toInt();
        p.userId     = q.value(1).toInt();
        p.name       = q.value(2).toString();
        p.subject    = q.value(3).toString();
        p.department = q.value(4).toString();
        p.days       = q.value(5).toString().split(',', Qt::SkipEmptyParts);
        p.startTime = QTime::fromString(q.value(6).toString(), "HH:mm:ss");
        p.endTime   = QTime::fromString(q.value(7).toString(), "HH:mm:ss");
        list.append(p);
    }
    return list;
}

} // namespace crm::services