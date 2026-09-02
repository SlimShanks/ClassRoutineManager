#pragma once
#include "database/IDatabase.h"
#include <QString>
#include <QStringList>
#include <QList>
#include <optional>
#include <QTime>

namespace crm::services {

struct TeacherProfile {
    int         id         = -1;
    int         userId     = -1;
    QString     name;
    QString     subject;
    QString     department;  // replaces room + section
    QStringList days;
    QTime       startTime;   // QTime instead of QString
    QTime       endTime;
};

class TeacherService {
public:
    explicit TeacherService(crm::database::Idatabase* db);

    // returns nullopt if teacher hasn't filled their profile yet
    std::optional<TeacherProfile> getProfile(int userId);

    bool createProfile(const TeacherProfile& p);
    bool updateProfile(const TeacherProfile& p);  // uses p.id as key

    QList<TeacherProfile> getAllProfiles();         // for admin view

private:
    crm::database::Idatabase* m_db;
};

} // namespace crm::services