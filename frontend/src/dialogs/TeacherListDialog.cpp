#include "TeacherListDialog.h"
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>

TeacherListDialog::TeacherListDialog(crm::AppContext* ctx, QWidget* parent)
    : QDialog(parent), m_ctx(ctx)
{
    setWindowTitle("All Teachers");
    resize(700, 440);
    setStyleSheet(R"(
        QDialog { background-color: #0f172a; }
        QTableWidget {
            background-color: #1e293b;
            color: #e2e8f0;
            border: none;
            gridline-color: #334155;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #0f172a;
            color: #64748b;
            font-size: 11px;
            font-weight: bold;
            padding: 6px;
            border: none;
            border-bottom: 1px solid #334155;
        }
        QTableWidget::item { padding: 8px; }
        QTableWidget::item:selected { background-color: #334155; }
        QPushButton#link {
            background: transparent;
            border: none;
            color: #38bdf8;
            font-size: 12px;
            padding: 8px 0;
        }
        QPushButton#link:hover { color: #7dd3fc; }
    )");

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(20, 20, 20, 20);
    lay->setSpacing(12);

    QLabel* title = new QLabel("Teachers");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");

    m_table = new QTableWidget();
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        "Name", "Subject", "Department", "Days", "Start", "End"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);

    QPushButton* btnClose = new QPushButton("Close");
    btnClose->setObjectName("link");

    lay->addWidget(title);
    lay->addWidget(m_table);
    lay->addWidget(btnClose, 0, Qt::AlignRight);

    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    populate();
}

void TeacherListDialog::populate()
{
    auto profiles = m_ctx->teacherService()->getAllProfiles();

    m_table->setRowCount(profiles.size());

    for (int i = 0; i < profiles.size(); ++i) {
        const auto& p = profiles[i];
        m_table->setItem(i, 0, new QTableWidgetItem(p.name));
        m_table->setItem(i, 1, new QTableWidgetItem(p.subject));
        m_table->setItem(i, 2, new QTableWidgetItem(p.department));
        m_table->setItem(i, 3, new QTableWidgetItem(p.days.join(", ")));
        m_table->setItem(i, 4, new QTableWidgetItem(p.startTime.toString("HH:mm")));
        m_table->setItem(i, 5, new QTableWidgetItem(p.endTime.toString("HH:mm")));
    }
}