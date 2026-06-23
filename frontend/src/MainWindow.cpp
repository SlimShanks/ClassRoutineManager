#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QTimeEdit>

#include "dialogs/AddTeacherDialog.h"
#include "dialogs/TeacherListDialog.h"

// ── shared styles ─────────────────────────────────────────────────────────────
namespace {

const char* kBg = "background-color: #0f172a;";

const char* kCard = R"(
    QFrame {
        background-color: #1e293b;
        border-radius: 16px;
    }
    QLabel {
        color: #e2e8f0;
        font-size: 13px;
    }
    QLineEdit, QComboBox {
        background-color: #0f172a;
        border: 1px solid #334155;
        border-radius: 8px;
        padding: 9px;
        color: white;
        font-size: 13px;
    }
    QLineEdit:focus, QComboBox:focus {
        border: 1px solid #38bdf8;
    }
    QCheckBox {
        color: #e2e8f0;
        font-size: 13px;
        spacing: 6px;
    }
    QPushButton#primary {
        background-color: #38bdf8;
        border: none;
        padding: 10px;
        border-radius: 8px;
        font-weight: bold;
        color: #0f172a;
    }
    QPushButton#primary:hover { background-color: #0ea5e9; }
    QPushButton#secondary {
        background-color: #334155;
        border: none;
        padding: 12px;
        border-radius: 8px;
        color: white;
        font-size: 13px;
    }
    QPushButton#secondary:hover { background-color: #475569; }
    QPushButton#link {
        background: transparent;
        border: none;
        color: #38bdf8;
        font-size: 12px;
        padding: 4px 0;
    }
    QPushButton#link:hover { color: #7dd3fc; }
)";

const char* kDialog = R"(
    QDialog {
        background-color: #0f172a;
    }
    QLabel {
        color: #e2e8f0;
        font-size: 13px;
    }
    QLineEdit {
        background-color: #1e293b;
        border: 1px solid #334155;
        border-radius: 8px;
        padding: 9px;
        color: white;
        font-size: 13px;
    }
    QLineEdit:focus {
        border: 1px solid #38bdf8;
    }
    QPushButton#primary {
        background-color: #38bdf8;
        border: none;
        padding: 10px;
        border-radius: 8px;
        font-weight: bold;
        color: #0f172a;
    }
    QPushButton#primary:hover { background-color: #0ea5e9; }
    QPushButton#link {
        background: transparent;
        border: none;
        color: #94a3b8;
        font-size: 12px;
        padding: 4px 0;
    }
    QPushButton#link:hover { color: #e2e8f0; }
)";

QLabel* sectionLabel(const QString& text)
{
    QLabel* l = new QLabel(text);
    l->setStyleSheet("color: #64748b; font-size: 11px; font-weight: bold;");
    return l;
}

} // namespace

// ═════════════════════════════════════════════════════════════════════════════
// MainWindow
// ═════════════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(crm::AppContext* ctx, QWidget* parent)
    : QMainWindow(parent), m_ctx(ctx)
{
    setWindowTitle("Class Routine Manager");
    resize(900, 640);
    setStyleSheet(kBg);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    setupLogin();            // index 0
    setupSignup();           // index 1
    setupAdminDashboard();   // index 2
    setupTeacherDashboard(); // index 3

    m_stack->setCurrentIndex(0);
}

// ── LOGIN (index 0) ───────────────────────────────────────────────────────────
void MainWindow::setupLogin()
{
    QWidget*     page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setAlignment(Qt::AlignCenter);

    QFrame* card = new QFrame();
    card->setFixedSize(360, 340);
    card->setStyleSheet(kCard);

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setContentsMargins(28, 28, 28, 28);
    lay->setSpacing(10);

    QLabel* title = new QLabel("Sign in");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");

    m_loginUser = new QLineEdit();
    m_loginUser->setPlaceholderText("Username");
    m_loginPass = new QLineEdit();
    m_loginPass->setPlaceholderText("Password");
    m_loginPass->setEchoMode(QLineEdit::Password);

    QPushButton* btnLogin = new QPushButton("Sign in");
    btnLogin->setObjectName("primary");
    QPushButton* btnGo = new QPushButton("Don't have an account? Sign up");
    btnGo->setObjectName("link");

    lay->addWidget(title);
    lay->addSpacing(8);
    lay->addWidget(m_loginUser);
    lay->addWidget(m_loginPass);
    lay->addSpacing(8);
    lay->addWidget(btnLogin);
    lay->addWidget(btnGo);

    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(btnGo,    &QPushButton::clicked, this,
            [this]{ m_stack->setCurrentIndex(1); });

    root->addWidget(card);
    m_stack->addWidget(page);
}

// ── SIGNUP (index 1) ──────────────────────────────────────────────────────────
void MainWindow::setupSignup()
{
    QWidget*     page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setAlignment(Qt::AlignCenter);

    QFrame* card = new QFrame();
    card->setFixedSize(360, 480);
    card->setStyleSheet(kCard);

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setContentsMargins(28, 28, 28, 28);
    lay->setSpacing(10);

    QLabel* title = new QLabel("Create account");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");

    m_regUser  = new QLineEdit(); m_regUser->setPlaceholderText("Username");
    m_regEmail = new QLineEdit(); m_regEmail->setPlaceholderText("Email address");
    m_regPass  = new QLineEdit(); m_regPass->setPlaceholderText("Password");
    m_regPass->setEchoMode(QLineEdit::Password);
    m_regPass2 = new QLineEdit(); m_regPass2->setPlaceholderText("Confirm password");
    m_regPass2->setEchoMode(QLineEdit::Password);

    m_regRole = new QComboBox();
    m_regRole->addItem("Teacher");
    m_regRole->addItem("Admin");

    QPushButton* btnReg = new QPushButton("Create account");
    btnReg->setObjectName("primary");
    QPushButton* btnGo  = new QPushButton("Already have an account? Sign in");
    btnGo->setObjectName("link");

    lay->addWidget(title);
    lay->addSpacing(6);
    lay->addWidget(m_regUser);
    lay->addWidget(m_regEmail);
    lay->addWidget(m_regPass);
    lay->addWidget(m_regPass2);
    lay->addWidget(sectionLabel("ACCOUNT TYPE"));
    lay->addWidget(m_regRole);
    lay->addSpacing(6);
    lay->addWidget(btnReg);
    lay->addWidget(btnGo);

    connect(btnReg, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(btnGo,  &QPushButton::clicked, this,
            [this]{ m_stack->setCurrentIndex(0); });

    root->addWidget(card);
    m_stack->addWidget(page);
}

// ── ADMIN DASHBOARD (index 2) ─────────────────────────────────────────────────
void MainWindow::setupAdminDashboard()
{
    QWidget*     page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setAlignment(Qt::AlignCenter);

    QFrame* card = new QFrame();
    card->setFixedSize(520, 420);
    card->setStyleSheet(kCard);

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(14);

    QLabel* title = new QLabel("Admin Dashboard");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: white;");

    QLabel* sub = new QLabel("Manage teachers and class routines");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color: #64748b; font-size: 13px;");

    m_btnAddTeacher  = new QPushButton("Add Teacher");
    m_btnAddTeacher->setObjectName("secondary");
    m_btnMakeRoutine = new QPushButton("Make Routine");
    m_btnMakeRoutine->setObjectName("secondary");
    QPushButton* btnViewTeachers = new QPushButton("View Teachers");
    btnViewTeachers->setObjectName("secondary");

    QPushButton* btnLogout = new QPushButton("Sign out");
    btnLogout->setObjectName("link");

    lay->addWidget(title);
    lay->addWidget(sub);
    lay->addSpacing(20);
    lay->addWidget(m_btnAddTeacher);
    lay->addWidget(m_btnMakeRoutine);
    lay->addWidget(btnViewTeachers);
    lay->addSpacing(10);
    lay->addWidget(btnLogout);

    connect(m_btnAddTeacher, &QPushButton::clicked, this, [this]{
        AddTeacherDialog dlg(m_ctx, this);
        dlg.exec();
    });
    connect(m_btnMakeRoutine, &QPushButton::clicked, this,
            []{ /* RoutineDialog — next */ });
    connect(btnViewTeachers, &QPushButton::clicked, this, [this]{
        TeacherListDialog dlg(m_ctx, this);
        dlg.exec();
    });
    connect(btnLogout, &QPushButton::clicked, this, [this]{
        m_loggedInUserId = -1;
        m_stack->setCurrentIndex(0);
    });

    root->addWidget(card);
    m_stack->addWidget(page);
}
// ── TEACHER DASHBOARD (index 3) ───────────────────────────────────────────────
void MainWindow::setupTeacherDashboard()
{
    QWidget*     page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setAlignment(Qt::AlignCenter);

    QFrame* card = new QFrame();
    card->setFixedSize(500, 540);
    card->setStyleSheet(kCard);

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setContentsMargins(28, 28, 28, 28);
    lay->setSpacing(10);

    QLabel* title = new QLabel("My Teaching Info");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");

    m_tName       = new QLineEdit(); m_tName->setPlaceholderText("Full name");
    m_tSubject    = new QLineEdit(); m_tSubject->setPlaceholderText("Subject");
    m_tDepartment = new QLineEdit(); m_tDepartment->setPlaceholderText("Department  e.g. CSE");

    m_tStart = new QTimeEdit(); m_tStart->setDisplayFormat("HH:mm");
    m_tStart->setTime(QTime(9, 0));
    m_tEnd   = new QTimeEdit(); m_tEnd->setDisplayFormat("HH:mm");
    m_tEnd->setTime(QTime(11, 0));

    QHBoxLayout* daysRow = new QHBoxLayout();
    daysRow->setSpacing(8);
    for (const QString& d : { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" }) {
        QCheckBox* cb = new QCheckBox(d);
        m_tDays.append(cb);
        daysRow->addWidget(cb);
    }

    m_tSubmit = new QPushButton("Save info");
    m_tSubmit->setObjectName("primary");

    QPushButton* btnLogout = new QPushButton("Sign out");
    btnLogout->setObjectName("link");

    lay->addWidget(title);
    lay->addSpacing(6);
    lay->addWidget(sectionLabel("PERSONAL INFO"));
    lay->addWidget(m_tName);
    lay->addWidget(sectionLabel("CLASS DETAILS"));
    lay->addWidget(m_tSubject);
    lay->addWidget(m_tDepartment);
    lay->addWidget(sectionLabel("AVAILABLE DAYS"));
    lay->addLayout(daysRow);
    lay->addWidget(sectionLabel("TIME SLOT"));
    QHBoxLayout* timeRow = new QHBoxLayout();
    timeRow->addWidget(m_tStart);
    timeRow->addWidget(new QLabel("to"));
    timeRow->addWidget(m_tEnd);
    lay->addLayout(timeRow);
    lay->addSpacing(8);
    lay->addWidget(m_tSubmit);
    lay->addWidget(btnLogout);

    connect(m_tSubmit, &QPushButton::clicked, this, &MainWindow::onTeacherSubmit);
    connect(btnLogout, &QPushButton::clicked, this, [this]{
        m_loggedInUserId   = -1;
        m_teacherProfileId = -1;
        m_stack->setCurrentIndex(0);
    });

    root->addWidget(card);
    m_stack->addWidget(page);
}

// ── load existing teacher profile ─────────────────────────────────────────────
void MainWindow::loadTeacherProfile()
{
    auto profile = m_ctx->teacherService()->getProfile(m_loggedInUserId);

    if (!profile) {
        m_teacherProfileId = -1;
        m_tName->clear();
        m_tSubject->clear();
        m_tDepartment->clear();
        m_tStart->setTime(QTime(9, 0));
        m_tEnd->setTime(QTime(11, 0));
        for (auto* cb : m_tDays) cb->setChecked(false);
        m_tSubmit->setText("Save info");
        return;
    }

    m_teacherProfileId = profile->id;
    m_tName->setText(profile->name);
    m_tSubject->setText(profile->subject);
    m_tDepartment->setText(profile->department);
    m_tStart->setTime(profile->startTime);
    m_tEnd->setTime(profile->endTime);

    const QStringList dayNames = { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
    for (int i = 0; i < m_tDays.size(); ++i)
        m_tDays[i]->setChecked(profile->days.contains(dayNames[i]));

    m_tSubmit->setText("Update info");
}

// ── LOGIN SLOT ────────────────────────────────────────────────────────────────
void MainWindow::onLoginClicked()
{
    const QString user = m_loginUser->text().trimmed();
    const QString pass = m_loginPass->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Sign in", "Please fill in both fields.");
        return;
    }

    LoginResult r = m_ctx->login(user, pass);

    if (!r.success) {
        QMessageBox::warning(this, "Sign in failed", "Wrong username or password.");
        return;
    }

    m_loggedInUserId = r.userId;

    if (r.role == Role::Admin) {
        m_stack->setCurrentIndex(2);
    } else {
        loadTeacherProfile();
        m_stack->setCurrentIndex(3);
    }
}

// ── REGISTER SLOT ─────────────────────────────────────────────────────────────
void MainWindow::onRegisterClicked()
{
    const QString user  = m_regUser->text().trimmed();
    const QString email = m_regEmail->text().trimmed();
    const QString pass  = m_regPass->text();
    const QString pass2 = m_regPass2->text();

    if (user.isEmpty() || email.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Sign up", "Please fill in all fields.");
        return;
    }
    if (pass != pass2) {
        QMessageBox::warning(this, "Sign up", "Passwords do not match.");
        return;
    }
    if (pass.length() < 6) {
        QMessageBox::warning(this, "Sign up",
                             "Password must be at least 6 characters.");
        return;
    }

    Role role = (m_regRole->currentText() == "Admin") ? Role::Admin : Role::Teacher;

    if (m_ctx->registerUser(user, email, pass, role)) {
        QMessageBox::information(this, "Account created", "You can now sign in.");
        m_stack->setCurrentIndex(0);
    } else {
        QMessageBox::warning(this, "Sign up failed",
                             "Username or email already taken.");
    }
}

// ── TEACHER SUBMIT SLOT ───────────────────────────────────────────────────────
void MainWindow::onTeacherSubmit()
{
    if (m_tName->text().trimmed().isEmpty()       ||
        m_tSubject->text().trimmed().isEmpty()    ||
        m_tDepartment->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Missing fields",
                             "Please fill in name, subject and department.");
        return;
    }

    if (m_tStart->time() >= m_tEnd->time()) {
        QMessageBox::warning(this, "Invalid time",
                             "Start time must be before end time.");
        return;
    }

    QStringList selectedDays;
    const QStringList dayNames = { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
    for (int i = 0; i < m_tDays.size(); ++i)
        if (m_tDays[i]->isChecked())
            selectedDays << dayNames[i];

    if (selectedDays.isEmpty()) {
        QMessageBox::warning(this, "Missing fields",
                             "Please select at least one day.");
        return;
    }

    crm::services::TeacherProfile p;
    p.userId     = m_loggedInUserId;
    p.id         = m_teacherProfileId;
    p.name       = m_tName->text().trimmed();
    p.subject    = m_tSubject->text().trimmed();
    p.department = m_tDepartment->text().trimmed();
    p.startTime  = m_tStart->time();
    p.endTime    = m_tEnd->time();
    p.days       = selectedDays;

    bool ok = (m_teacherProfileId == -1)
        ? m_ctx->teacherService()->createProfile(p)
        : m_ctx->teacherService()->updateProfile(p);

    if (ok) {
        QMessageBox::information(this, "Saved", "Your info has been saved.");
        loadTeacherProfile();
    } else {
        QMessageBox::warning(this, "Error", "Could not save. Please try again.");
    }
}