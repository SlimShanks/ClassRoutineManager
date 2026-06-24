#pragma once
#include <QFrame>
#include <QPoint>
#include "../../../backend/src/services/TeacherService.h"
#include "../../../backend/src/services/RoutineService.h"

class QMouseEvent;
class QContextMenuEvent;

class TeacherBlockWidget : public QFrame {
    Q_OBJECT
public:
    explicit TeacherBlockWidget(const crm::services::TeacherProfile& profile,
                                QWidget* parent = nullptr);
    int teacherId() const { return m_teacherId; }
    QString teacherName() const { return m_name; }
    QString subject() const { return m_subject; }

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    int     m_teacherId;
    QString m_name;
    QString m_subject;
    QPoint  m_dragStart;
};

class GridBlockWidget : public QFrame {
    Q_OBJECT
public:
    explicit GridBlockWidget(const crm::services::RoutineSlot& slot,
                             QWidget* parent = nullptr);
    int slotId() const { return m_slotId; }
    int teacherId() const { return m_teacherId; }

signals:
    void deleteRequested(int slotId);
    void resizeFinished(int slotId, int newRowSpan);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;

private:
    int  m_slotId;
    int  m_teacherId;
    bool m_resizing    = false;
    int  m_resizeStartY = 0;
    int  m_startHeight  = 0;
};