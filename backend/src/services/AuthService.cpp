#include "AuthService.h"
#include <QCryptographicHash>
#include <QSqlQuery>

namespace crm::services {

AuthService::AuthService(crm::database::Idatabase* db) : m_db(db) {}

static QString hashPassword(const QString& password)
{
    return QString(QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

LoginResult AuthService::login(const QString& username, const QString& password)
{
    QSqlQuery q = m_db->query(
        "SELECT id, role FROM users WHERE name = ? AND password_hash = ?",
        { username, hashPassword(password) }
    );

    if (!q.next()) return {};

    LoginResult r;
    r.success = true;
    r.userId  = q.value(0).toInt();
    r.role    = (q.value(1).toString() == "admin") ? Role::Admin : Role::Teacher;
    return r;
}

bool AuthService::registerUser(const QString& username, const QString& email,
                               const QString& password, Role role)
{
    QSqlQuery check = m_db->query(
        "SELECT COUNT(*) FROM users WHERE name = ? OR email = ?",
        { username, email }
    );
    if (check.next() && check.value(0).toInt() > 0)
        return false;

    QString roleStr = (role == Role::Admin) ? "admin" : "teacher";
    return m_db->execute(
        "INSERT INTO users (name, email, role, password_hash) VALUES (?, ?, ?, ?)",
        { username, email, roleStr, hashPassword(password) }
    );
}

} // namespace crm::services