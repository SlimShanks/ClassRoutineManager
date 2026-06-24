#include "TeacherBlockWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QMenu>
#include <QJsonObject>
#include <QJsonDocument>

TeacherBlockWidget::TeacherBlockWidget(const crm::services::TeacherProfile& p, QWidget* parent)
    : QFrame(parent), m_teacherId(p.id), m_name(p.name), m_subject(p.subject)
{
    setFixedSize(160, 64);
    setCursor(Qt::OpenHandCursor);
    setStyleSheet(
        "QFrame { background-color: #1e40af; border-radius: 8px; border: 1px solid #3b82f6; }"
        "QLabel { color: white; background: transparent; border: none; }"
    );
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 8, 10, 8);
    lay->setSpacing(2);

    QLabel* name = new QLabel(p.name);
    name->setStyleSheet("font-weight: bold; font-size: 12px;");
    name->setWordWrap(true);

    QLabel* subj = new QLabel(p.subject);
    subj->setStyleSheet("font-size: 11px; color: #93c5fd;");

    lay->addWidget(name);
    lay->addWidget(subj);
}

void TeacherBlockWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_dragStart = e->pos();
    QFrame::mousePressEvent(e);
}

void TeacherBlockWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (!(e->buttons() & Qt::LeftButton)) return;
    if ((e->pos() - m_dragStart).manhattanLength() < 10) return;

    QJsonObject obj;
    obj["teacherId"] = m_teacherId;
    obj["name"]      = m_name;
    obj["subject"]   = m_subject;

    QMimeData* mime = new QMimeData();
    mime->setData("application/x-teacher-block", QJsonDocument(obj).toJson());

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mime);

    QPixmap px(size());
    render(&px);
    drag->setPixmap(px);
    drag->setHotSpot(e->pos());
    drag->exec(Qt::CopyAction);
}

GridBlockWidget::GridBlockWidget(const crm::services::RoutineSlot& slot, QWidget* parent)
    : QFrame(parent), m_slotId(slot.id), m_teacherId(slot.teacherId)
{
    setStyleSheet(
        "QFrame { background-color: #065f46; border-radius: 6px; border: 1px solid #10b981; }"
        "QLabel { color: white; background: transparent; border: none; }"
    );
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 6, 8, 6);
    lay->setSpacing(2);

    QLabel* name = new QLabel(slot.teacherName);
    name->setStyleSheet("font-weight: bold; font-size: 11px;");
    name->setWordWrap(true);

    QLabel* subj = new QLabel(slot.subject);
    subj->setStyleSheet("font-size: 10px; color: #6ee7b7;");

    QLabel* time = new QLabel(
        slot.startTime.toString("HH:mm") + " - " + slot.endTime.toString("HH:mm")
    );
    time->setStyleSheet("font-size: 10px; color: #a7f3d0;");

    lay->addWidget(name);
    lay->addWidget(subj);
    lay->addWidget(time);
}

void GridBlockWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        if (e->pos().y() > height() - 12) {
            m_resizing      = true;
            m_resizeStartY  = e->globalPosition().toPoint().y();
            m_startHeight   = height();
            setCursor(Qt::SizeVerCursor);
        }
    }
    QFrame::mousePressEvent(e);
}

void GridBlockWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_resizing) {
        int dy = e->globalPosition().toPoint().y() - m_resizeStartY;
        int newHeight = qMax(40, m_startHeight + dy);
        resize(width(), newHeight);
    }
    QFrame::mouseMoveEvent(e);
}

void GridBlockWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_resizing) {
        m_resizing = false;
        setCursor(Qt::SizeVerCursor);
        int rowSpan = qMax(1, height() / 40);
        emit resizeFinished(m_slotId, rowSpan);
    }
    QFrame::mouseReleaseEvent(e);
}

void GridBlockWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background: #1e293b; border: 1px solid #334155; border-radius: 6px; }"
        "QMenu::item { color: #e2e8f0; padding: 8px 16px; font-size: 13px; }"
        "QMenu::item:selected { background: #334155; }"
    );
    QAction* del = menu.addAction("Remove from routine");
    if (menu.exec(e->globalPos()) == del)
        emit deleteRequested(m_slotId);
}