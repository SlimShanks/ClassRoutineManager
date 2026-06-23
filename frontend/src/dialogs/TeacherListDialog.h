#pragma once
#include <QDialog>
#include "../../../backend/src/appcontext.h"

class QTableWidget;

class TeacherListDialog : public QDialog {
    Q_OBJECT
public:
    explicit TeacherListDialog(crm::AppContext* ctx, QWidget* parent = nullptr);

private:
    void populate();
    crm::AppContext* m_ctx;
    QTableWidget*    m_table;
};