#pragma once
#include <QWidget>
#include <QStackedWidget>
#include "../../../backend/src/appcontext.h"

class RoutineSetupWidget;
class RoutineGridWidget;

class RoutineView : public QWidget {
    Q_OBJECT
public:
    explicit RoutineView(crm::AppContext* ctx, QWidget* parent = nullptr);

signals:
    void backRequested();

private:
    void showSetup();
    void showGrid(int routineId);

    crm::AppContext*    m_ctx;
    QStackedWidget*     m_inner;
    RoutineSetupWidget* m_setup;
    RoutineGridWidget*  m_grid = nullptr;
};