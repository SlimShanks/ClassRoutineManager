#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QTime>
#include <QStringList>
#include "TeacherBlockWidget.h"
#include "../../../backend/src/appcontext.h"
#include "../../../backend/src/services/RoutineService.h"

class RoutineGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit RoutineGridWidget(crm::AppContext* ctx, int routineId, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    void reload();
    void placeBlock(int slotId, int teacherId, const QString& name, const QString& subject,
                    const QString& day, const QTime& start, const QTime& end);

    crm::AppContext* m_ctx;
    int m_routineId;

    static constexpr int kRowH  = 40;
    static constexpr int kColW  = 110;
    static constexpr int kHdrH  = 36;
    static constexpr int kTimeW = 60;

    const QStringList m_days;
    const QTime       m_start;
    const QTime       m_end;

    QWidget*     m_gridArea;
    QScrollArea* m_gridScroll;
};