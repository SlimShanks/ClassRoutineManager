#include "AddTeacherDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QPushButton> 

namespace {
const char* kDlgStyle = R"(
    QDialog, QWidget {
        background-color: #0f172a;
    }
    QLabel {
        color: #e2e8f0;
        font-size: 13px;
    }
    QLineEdit, QTimeEdit {
        background-color: #1e293b;
        border: 1px solid #334155;
        border-radius: 8px;
        padding: 9px;
        color: white;
        font-size: 13px;
    }
    QLineEdit:focus, QTimeEdit:focus { border: 1px solid #38bdf8; }
    QCheckBox { color: #e2e8f0; font-size: 13px; spacing: 6px; }
    QPushButton#primary {
        background-color: #38bdf8;
        border: none; padding: 10px;
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

AddTeacherDialog::AddTeacherDialog(crm::AppContext* ctx, QWidget* parent)
    : QDialog(parent), m_ctx(ctx)
{
    setWindowTitle("Add Teacher");
    setFixedSize(420, 620);
    setStyleSheet(kDlgStyle);

    // scrollable in case window is small
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget*     inner = new QWidget();
    QVBoxLayout* lay   = new QVBoxLayout(inner);
    lay->setContentsMargins(28, 28, 28, 28);
    lay->setSpacing(10);

    QLabel* title = new QLabel("Add Teacher");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");

    // ── account fields ────────────────────────────────────────────────────────
    m_username = new QLineEdit(); m_username->setPlaceholderText("Username");
    m_email    = new QLineEdit(); m_email->setPlaceholderText("Email address");
    m_pass     = new QLineEdit(); m_pass->setPlaceholderText("Password");
    m_pass->setEchoMode(QLineEdit::Password);
    m_pass2    = new QLineEdit(); m_pass2->setPlaceholderText("Confirm password");
    m_pass2->setEchoMode(QLineEdit::Password);

    // ── profile fields ────────────────────────────────────────────────────────
    m_name       = new QLineEdit(); m_name->setPlaceholderText("Full name");
    m_subject    = new QLineEdit(); m_subject->setPlaceholderText("Subject");
    m_department = new QLineEdit(); m_department->setPlaceholderText("Department  e.g. CSE");

    m_start = new QTimeEdit(); m_start->setDisplayFormat("HH:mm");
    m_start->setTime(QTime(9, 0));
    m_end   = new QTimeEdit(); m_end->setDisplayFormat("HH:mm");
    m_end->setTime(QTime(11, 0));

    QHBoxLayout* daysRow = new QHBoxLayout();
    daysRow->setSpacing(6);
    for (const QString& d : { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" }) {
        QCheckBox* cb = new QCheckBox(d);
        m_days.append(cb);
        daysRow->addWidget(cb);
    }

    QPushButton* btnCreate = new QPushButton("Add Teacher");
    btnCreate->setObjectName("primary");
    QPushButton* btnCancel = new QPushButton("Cancel");
    btnCancel->setObjectName("link");

    lay->addWidget(title);
    lay->addSpacing(8);
    lay->addWidget(sectionLabel("LOGIN CREDENTIALS"));
    lay->addWidget(m_username);
    lay->addWidget(m_email);
    lay->addWidget(m_pass);
    lay->addWidget(m_pass2);
    lay->addSpacing(8);
    lay->addWidget(sectionLabel("TEACHING INFO"));
    lay->addWidget(m_name);
    lay->addWidget(m_subject);
    lay->addWidget(m_department);
    lay->addWidget(sectionLabel("AVAILABLE DAYS"));
    lay->addLayout(daysRow);
    lay->addWidget(sectionLabel("TIME SLOT"));
    QHBoxLayout* timeRow = new QHBoxLayout();
    timeRow->addWidget(m_start);
    timeRow->addWidget(new QLabel("to"));
    timeRow->addWidget(m_end);
    lay->addLayout(timeRow);
    lay->addSpacing(12);
    lay->addWidget(btnCreate);
    lay->addWidget(btnCancel);

    scroll->setWidget(inner);
    root->addWidget(scroll);

    connect(btnCreate, &QPushButton::clicked, this, &AddTeacherDialog::onSubmit);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void AddTeacherDialog::onSubmit()
{
    const QString user  = m_username->text().trimmed();
    const QString email = m_email->text().trimmed();
    const QString pass  = m_pass->text();
    const QString pass2 = m_pass2->text();
    const QString name  = m_name->text().trimmed();
    const QString subj  = m_subject->text().trimmed();
    const QString dept  = m_department->text().trimmed();

    if (user.isEmpty() || email.isEmpty() || pass.isEmpty() ||
        name.isEmpty() || subj.isEmpty()  || dept.isEmpty())
    {
        QMessageBox::warning(this, "Missing fields", "Please fill in all fields.");
        return;
    }
    if (pass != pass2) {
        QMessageBox::warning(this, "Password mismatch", "Passwords do not match.");
        return;
    }
    if (pass.length() < 6) {
        QMessageBox::warning(this, "Weak password",
                             "Password must be at least 6 characters.");
        return;
    }
    if (m_start->time() >= m_end->time()) {
        QMessageBox::warning(this, "Invalid time",
                             "Start time must be before end time.");
        return;
    }

    QStringList selectedDays;
    const QStringList dayNames = { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
    for (int i = 0; i < m_days.size(); ++i)
        if (m_days[i]->isChecked())
            selectedDays << dayNames[i];

    if (selectedDays.isEmpty()) {
        QMessageBox::warning(this, "Missing fields",
                             "Please select at least one available day.");
        return;
    }

    // 1. create the user account
    if (!m_ctx->registerUser(user, email, pass, Role::Teacher)) {
        QMessageBox::warning(this, "Failed",
                             "Username or email is already taken.");
        return;
    }

    // 2. fetch the new user's id then create their profile
    LoginResult r = m_ctx->login(user, pass);
    if (!r.success) {
        QMessageBox::warning(this, "Error",
                             "Account created but could not load it. "
                             "Please check the database.");
        return;
    }

    crm::services::TeacherProfile p;
    p.userId     = r.userId;
    p.name       = name;
    p.subject    = subj;
    p.department = dept;
    p.days       = selectedDays;
    p.startTime  = m_start->time();
    p.endTime    = m_end->time();

    if (!m_ctx->teacherService()->createProfile(p)) {
        QMessageBox::warning(this, "Error",
                             "Account created but profile could not be saved.");
        return;
    }

    QMessageBox::information(this, "Done",
        QString("Teacher \"%1\" added successfully.").arg(name));
    accept();
}