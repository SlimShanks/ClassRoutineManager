#pragma once
#include "database/IDatabase.h"
#include <QString>

enum class Role { Admin, Teacher, Unknown };

struct LoginResult {
    bool  success = false;
    Role  role    = Role::Unknown;
    int   userId  = -1;
};

namespace crm::services {

class AuthService {
public:
    explicit AuthService(crm::database::Idatabase* db);

    LoginResult login(const QString& username, const QString& password);
    bool        registerUser(const QString& username, const QString& email,
                             const QString& password, Role role);

private:
    crm::database::Idatabase* m_db;
};

} // namespace crm::services