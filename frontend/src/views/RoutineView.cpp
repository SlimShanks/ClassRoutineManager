#include "RoutineView.h"
#include "RoutineSetupWidget.h"
#include "RoutineGridWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

RoutineView::RoutineView(crm::AppContext* ctx, QWidget* parent)
    : QWidget(parent), m_ctx(ctx)
{
    setStyleSheet("background-color: #0f172a;");

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget* bar = new QWidget();
    bar->setFixedHeight(52);
    bar->setStyleSheet("background-color: #1e293b; border-bottom: 1px solid #334155;");
    QHBoxLayout* barLay = new QHBoxLayout(bar);
    barLay->setContentsMargins(16, 0, 16, 0);

    QPushButton* btnBack = new QPushButton("<- Back");
    btnBack->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #38bdf8; font-size: 13px; padding: 6px 10px; }"
        "QPushButton:hover { color: #7dd3fc; }"
    );

    QLabel* barTitle = new QLabel("Routine Manager");
    barTitle->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");

    barLay->addWidget(btnBack);
    barLay->addWidget(barTitle);
    barLay->addStretch();

    m_inner = new QStackedWidget();
    m_setup = new RoutineSetupWidget(m_ctx);
    m_inner->addWidget(m_setup);

    root->addWidget(bar);
    root->addWidget(m_inner, 1);

    connect(btnBack, &QPushButton::clicked, this, [this]{
        if (m_inner->currentIndex() == 1)
            showSetup();
        else
            emit backRequested();
    });

    connect(m_setup, &RoutineSetupWidget::routineSelected,
            this, &RoutineView::showGrid);
}

void RoutineView::showSetup()
{
    m_inner->setCurrentIndex(0);
}

void RoutineView::showGrid(int routineId)
{
    if (m_grid) {
        m_inner->removeWidget(m_grid);
        delete m_grid;
        m_grid = nullptr;
    }
    m_grid = new RoutineGridWidget(m_ctx, routineId);
    m_inner->addWidget(m_grid);
    m_inner->setCurrentWidget(m_grid);
}