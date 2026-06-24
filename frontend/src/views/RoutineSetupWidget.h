#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QListWidget>
#include "../../../backend/src/appcontext.h"

class RoutineSetupWidget : public QWidget {
    Q_OBJECT
public:
    explicit RoutineSetupWidget(crm::AppContext* ctx, QWidget* parent = nullptr);

signals:
    void routineSelected(int routineId);

private slots:
    void onCreateClicked();
    void onOpenClicked();

private:
    void refreshList();

    crm::AppContext* m_ctx;
    QLineEdit*   m_program;
    QLineEdit*   m_section;
    QSpinBox*    m_semester;
    QSpinBox*    m_year;
    QListWidget* m_list;
};