#include "RoutineSetupWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>

RoutineSetupWidget::RoutineSetupWidget(crm::AppContext* ctx, QWidget* parent)
    : QWidget(parent), m_ctx(ctx)
{
    setStyleSheet(
        "QWidget { background-color: #0f172a; }"
        "QLabel { color: #e2e8f0; font-size: 13px; }"
        "QLineEdit, QSpinBox {"
        "  background-color: #1e293b; border: 1px solid #334155;"
        "  border-radius: 8px; padding: 9px; color: white; font-size: 13px; }"
        "QLineEdit:focus, QSpinBox:focus { border: 1px solid #38bdf8; }"
        "QListWidget {"
        "  background-color: #1e293b; border: 1px solid #334155;"
        "  border-radius: 8px; color: #e2e8f0; font-size: 13px; padding: 4px; }"
        "QListWidget::item { padding: 10px; border-radius: 6px; }"
        "QListWidget::item:selected { background-color: #334155; }"
        "QPushButton#primary {"
        "  background-color: #38bdf8; border: none; padding: 10px;"
        "  border-radius: 8px; font-weight: bold; color: #0f172a; }"
        "QPushButton#primary:hover { background-color: #0ea5e9; }"
        "QPushButton#secondary {"
        "  background-color: #334155; border: none; padding: 10px;"
        "  border-radius: 8px; color: white; font-size: 13px; }"
        "QPushButton#secondary:hover { background-color: #475569; }"
    );

    QHBoxLayout* root = new QHBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(40);

    QWidget* left = new QWidget();
    QVBoxLayout* leftLay = new QVBoxLayout(left);
    leftLay->setSpacing(10);

    QLabel* createTitle = new QLabel("New Routine");
    createTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");

    m_program = new QLineEdit();
    m_program->setPlaceholderText("Program  e.g. CSE");
    m_section = new QLineEdit();
    m_section->setPlaceholderText("Section  e.g. A");

    m_semester = new QSpinBox();
    m_semester->setRange(1, 8);
    m_semester->setPrefix("Semester  ");

    m_year = new QSpinBox();
    m_year->setRange(2020, 2050);
    m_year->setValue(2025);
    m_year->setPrefix("Year  ");

    QPushButton* btnCreate = new QPushButton("Create Routine");
    btnCreate->setObjectName("primary");

    leftLay->addWidget(createTitle);
    leftLay->addSpacing(8);
    leftLay->addWidget(m_program);
    leftLay->addWidget(m_section);
    leftLay->addWidget(m_semester);
    leftLay->addWidget(m_year);
    leftLay->addSpacing(8);
    leftLay->addWidget(btnCreate);
    leftLay->addStretch();

    QFrame* divider = new QFrame();
    divider->setFixedWidth(1);
    divider->setStyleSheet("background-color: #334155;");
    divider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QWidget* right = new QWidget();
    QVBoxLayout* rightLay = new QVBoxLayout(right);
    rightLay->setSpacing(10);

    QLabel* existTitle = new QLabel("Existing Routines");
    existTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");

    m_list = new QListWidget();

    QPushButton* btnOpen = new QPushButton("Open Selected");
    btnOpen->setObjectName("secondary");

    rightLay->addWidget(existTitle);
    rightLay->addSpacing(8);
    rightLay->addWidget(m_list);
    rightLay->addWidget(btnOpen);

    root->addWidget(left);
    root->addWidget(divider);
    root->addWidget(right);

    connect(btnCreate, &QPushButton::clicked, this, &RoutineSetupWidget::onCreateClicked);
    connect(btnOpen, &QPushButton::clicked, this, &RoutineSetupWidget::onOpenClicked);
    connect(m_list, &QListWidget::itemDoubleClicked, this, &RoutineSetupWidget::onOpenClicked);

    refreshList();
}

void RoutineSetupWidget::refreshList()
{
    m_list->clear();
    auto routines = m_ctx->routineService()->getAllRoutines();
    for (const auto& r : routines) {
        QString label = QString("%1-%2  |  Semester %3  |  %4")
            .arg(r.program, r.section)
            .arg(r.semester)
            .arg(r.year);
        QListWidgetItem* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, r.id);
        m_list->addItem(item);
    }
}

void RoutineSetupWidget::onCreateClicked()
{
    if (m_program->text().trimmed().isEmpty() || m_section->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Please enter program and section.");
        return;
    }
    crm::services::Routine r;
    r.program  = m_program->text().trimmed().toUpper();
    r.section  = m_section->text().trimmed().toUpper();
    r.semester = m_semester->value();
    r.year     = m_year->value();
    int id = m_ctx->routineService()->createRoutine(r);
    if (id == -1) {
        QMessageBox::warning(this, "Failed", "Could not create routine. It may already exist.");
        return;
    }
    refreshList();
    emit routineSelected(id);
}

void RoutineSetupWidget::onOpenClicked()
{
    QListWidgetItem* item = m_list->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Nothing selected", "Please select a routine from the list.");
        return;
    }
    emit routineSelected(item->data(Qt::UserRole).toInt());
}