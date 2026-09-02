#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTimeEdit>
#include <QList>
#include "../../../backend/src/appcontext.h"
#include "../../../backend/src/services/TeacherService.h"

class AddTeacherDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddTeacherDialog(crm::AppContext* ctx, QWidget* parent = nullptr);

private slots:
    void onSubmit();

private:
    crm::AppContext* m_ctx;

    // account
    QLineEdit* m_username;
    QLineEdit* m_email;
    QLineEdit* m_pass;
    QLineEdit* m_pass2;

    // profile
    QLineEdit*        m_name;
    QLineEdit*        m_subject;
    QLineEdit*        m_department;
    QTimeEdit*        m_start;
    QTimeEdit*        m_end;
    QList<QCheckBox*> m_days;
};