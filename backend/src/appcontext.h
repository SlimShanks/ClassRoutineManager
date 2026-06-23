#pragma once
#include <memory>
#include "Config.h"
#include "database/IDatabase.h"
#include "services/AuthService.h"
#include "services/TeacherService.h"

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

private:
    Config m_config;
    std::unique_ptr<database::Idatabase>      m_db;
    std::unique_ptr<services::AuthService>    m_auth;
    std::unique_ptr<services::TeacherService> m_teachers;
    bool m_ready = false;
};

} // namespace crm