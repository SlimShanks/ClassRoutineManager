#pragma once
#include <memory>
#include "Config.h"
#include "database/IDatabase.h"
#include "services/AuthService.h"
#include "services/TeacherService.h"
#include "services/RoutineService.h"


namespace crm {

class AppContext {
public:
    explicit AppContext(Config config);
    ~AppContext();

    bool init();
    bool isReady() const;

    // returns LoginResult so frontend knows which dashboard to show
    LoginResult login(const QString& username, const QString& password);
    bool        registerUser(const QString& username, const QString& email,
                             const QString& password, Role role);

    services::TeacherService* teacherService();
    database::Idatabase*      db();
    services::RoutineService* routineService();


private:
    Config m_config;
    std::unique_ptr<database::Idatabase>      m_db;
    std::unique_ptr<services::AuthService>    m_auth;
    std::unique_ptr<services::TeacherService> m_teachers;
    std::unique_ptr<services::RoutineService> m_routine;

    bool m_ready = false;
};

} // namespace crm