#include "appcontext.h"
#include "database/MysqlDB.h"
#include "utils/Logger.h"

namespace crm {

AppContext::AppContext(Config config) : m_config(std::move(config)) {}
AppContext::~AppContext() = default;

bool AppContext::init()
{
    database::Config mc;
    mc.host     = m_config.host;
    mc.port     = m_config.port;
    mc.user     = m_config.user;
    mc.password = m_config.password;
    mc.database = m_config.database;

    m_db = std::make_unique<database::Mysqldb>(mc);

    if (!m_db->open()) {
        utils::Logger::error("DB open failed");
        return false;
    }
    if (!m_db->migrate()) {
        utils::Logger::error("DB migrate failed");
        return false;
    }

    m_auth      = std::make_unique<services::AuthService>(m_db.get());
    m_teachers  = std::make_unique<services::TeacherService>(m_db.get());
    m_routine   = std::make_unique<services::RoutineService>(m_db.get());
    m_conflicts = std::make_unique<services::ConflictService>(m_db.get());

    m_ready = true;
    utils::Logger::info("Backend ready");
    return true;
}

bool AppContext::isReady() const { return m_ready; }

LoginResult AppContext::login(const QString& username, const QString& password)
{
    return m_auth->login(username, password);
}

bool AppContext::registerUser(const QString& username, const QString& email,
                              const QString& password, Role role)
{
    return m_auth->registerUser(username, email, password, role);
}

services::TeacherService*  AppContext::teacherService()  { return m_teachers.get(); }
database::Idatabase*       AppContext::db()              { return m_db.get(); }
services::RoutineService*  AppContext::routineService()  { return m_routine.get(); }
services::ConflictService* AppContext::conflictService() { return m_conflicts.get(); }

} // namespace crm