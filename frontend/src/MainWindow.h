#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTimeEdit>
#include <QLabel>

#include "../../backend/src/appcontext.h"
#include "../../backend/src/services/TeacherService.h"
#include "views/RoutineView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(crm::AppContext* ctx, QWidget* parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onTeacherSubmit();

private:
    void setupLogin();
    void setupSignup();
    void setupAdminDashboard();
    void setupTeacherDashboard();
    void loadTeacherProfile();

    crm::AppContext* m_ctx;
    QStackedWidget*  m_stack;
    int              m_loggedInUserId   = -1;
    int              m_teacherProfileId = -1;

    // login (index 0)
    QLineEdit* m_loginUser;
    QLineEdit* m_loginPass;

    // signup (index 1)
    QLineEdit* m_regUser;
    QLineEdit* m_regEmail;
    QLineEdit* m_regPass;
    QLineEdit* m_regPass2;
    QComboBox* m_regRole;

    // admin dashboard (index 2)
    QPushButton* m_btnAddTeacher;
    QPushButton* m_btnMakeRoutine;

    // teacher dashboard (index 3)
    QLineEdit*        m_tName;
    QLineEdit*        m_tSubject;
    QLineEdit*        m_tDepartment;
    QTimeEdit*        m_tStart;
    QTimeEdit*        m_tEnd;
    QList<QCheckBox*> m_tDays;
    QPushButton*      m_tSubmit;

    // routine view (index 4)
    RoutineView* m_routineView;
};