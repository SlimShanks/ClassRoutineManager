#include "RoutineGridWidget.h"
#include "TeacherBlockWidget.h"
#include "../../../backend/src/services/ConflictService.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>

RoutineGridWidget::RoutineGridWidget(crm::AppContext* ctx, int routineId, QWidget* parent)
    : QWidget(parent)
    , m_ctx(ctx)
    , m_routineId(routineId)
    , m_days({"Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"})
    , m_start(QTime(7, 0))
    , m_end(QTime(17, 0))
    , m_gridArea(nullptr)
    , m_gridScroll(nullptr)
{
    setStyleSheet("background-color: #0f172a;");
    QHBoxLayout* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget* panel = new QWidget();
    panel->setFixedWidth(190);
    panel->setStyleSheet("background-color: #0f172a; border-right: 1px solid #1e293b;");
    QVBoxLayout* panelLay = new QVBoxLayout(panel);
    panelLay->setContentsMargins(12, 12, 12, 12);
    panelLay->setSpacing(8);

    QLabel* panelTitle = new QLabel("Teachers");
    panelTitle->setStyleSheet("color: #64748b; font-size: 11px; font-weight: bold;");
    panelLay->addWidget(panelTitle);

    auto profiles = m_ctx->teacherService()->getAllProfiles();
    for (const auto& p : profiles) {
        TeacherBlockWidget* block = new TeacherBlockWidget(p, panel);
        panelLay->addWidget(block);
    }
    panelLay->addStretch();

    m_gridScroll = new QScrollArea();
    m_gridScroll->setWidgetResizable(false);
    m_gridScroll->setStyleSheet("border: none; background: #0f172a;");

    int totalRows = m_start.secsTo(m_end) / 1800;
    int gridW = kTimeW + m_days.size() * kColW;
    int gridH = kHdrH + totalRows * kRowH;

    m_gridArea = new QWidget();
    m_gridArea->setFixedSize(gridW, gridH);
    m_gridArea->setAcceptDrops(true);
    m_gridArea->installEventFilter(this);

    for (int c = 0; c < m_days.size(); ++c) {
        QLabel* hdr = new QLabel(m_days[c], m_gridArea);
        hdr->setGeometry(kTimeW + c * kColW, 0, kColW, kHdrH);
        hdr->setAlignment(Qt::AlignCenter);
        hdr->setStyleSheet("color: #94a3b8; font-size: 12px; font-weight: bold; background: transparent;");
    }

    QTime t = m_start;
    int row = 0;
    while (t < m_end) {
        if (t.minute() == 0) {
            QLabel* lbl = new QLabel(t.toString("h:mm"), m_gridArea);
            lbl->setGeometry(0, kHdrH + row * kRowH, kTimeW, kRowH);
            lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            lbl->setStyleSheet("color: #475569; font-size: 11px; padding-right: 6px; background: transparent;");
        }
        t = t.addSecs(1800);
        ++row;
    }

    m_gridScroll->setWidget(m_gridArea);
    root->addWidget(panel);
    root->addWidget(m_gridScroll, 1);

    reload();
}

bool RoutineGridWidget::eventFilter(QObject* watched, QEvent* ev)
{
    if (watched != m_gridArea) {
        return QWidget::eventFilter(watched, ev);
    }

    if (ev->type() == QEvent::Paint) {
        QPainter p(m_gridArea);
        p.fillRect(m_gridArea->rect(), QColor("#0f172a"));
        QPen pen(QColor("#1e293b"));
        pen.setWidth(1);
        p.setPen(pen);
        int rows = m_start.secsTo(m_end) / 1800;
        for (int r = 0; r <= rows; ++r) {
            int y = kHdrH + r * kRowH;
            p.drawLine(kTimeW, y, kTimeW + m_days.size() * kColW, y);
        }
        for (int c = 0; c <= m_days.size(); ++c) {
            int x = kTimeW + c * kColW;
            p.drawLine(x, 0, x, kHdrH + rows * kRowH);
        }
        return true;
    }

    if (ev->type() == QEvent::DragEnter) {
        QDragEnterEvent* e = static_cast<QDragEnterEvent*>(ev);
        if (e->mimeData()->hasFormat("application/x-teacher-block")) {
            e->acceptProposedAction();
        }
        return true;
    }

    if (ev->type() == QEvent::DragMove) {
        static_cast<QDragMoveEvent*>(ev)->acceptProposedAction();
        return true;
    }

    if (ev->type() == QEvent::Drop) {
        QDropEvent* e = static_cast<QDropEvent*>(ev);
        QJsonObject data = QJsonDocument::fromJson(
            e->mimeData()->data("application/x-teacher-block")).object();

        int teacherId = data["teacherId"].toInt();
        QString name = data["name"].toString();
        QString subject = data["subject"].toString();

        QPoint pos = e->position().toPoint();
        int col = (pos.x() - kTimeW) / kColW;
        int row = (pos.y() - kHdrH) / kRowH;

        if (col < 0 || col >= m_days.size()) return true;
        if (row < 0) return true;

        QTime slotStart = m_start.addSecs(row * 1800);
        QTime slotEnd = slotStart.addSecs(3600);
        if (slotEnd > m_end) slotEnd = m_end;

        crm::services::RoutineSlot slot;
        slot.routineId   = m_routineId;
        slot.teacherId   = teacherId;
        slot.teacherName = name;
        slot.subject     = subject;
        slot.day         = m_days[col];
        slot.startTime   = slotStart;
        slot.endTime     = slotEnd;

        // ── conflict check: same teacher, same day, overlapping time, any routine ──
        auto conflicts = m_ctx->conflictService()->checkConflict(
            teacherId, slot.day, slotStart, slotEnd);

        if (!conflicts.isEmpty()) {
            const auto& c = conflicts.first();
            QMessageBox::warning(this, "Teacher Conflict",
                QString("%1 is already booked on %2 from %3 to %4\n(%5 - %6)")
                    .arg(name, c.day,
                         c.startTime.toString("h:mm ap"),
                         c.endTime.toString("h:mm ap"),
                         c.program, c.section));
            return true;
        }

        if (!m_ctx->routineService()->addSlot(slot)) {
            QMessageBox::warning(this, "Error", "Could not save slot.");
            return true;
        }

        reload();
        e->acceptProposedAction();
        return true;
    }

    return QWidget::eventFilter(watched, ev);
}

void RoutineGridWidget::reload()
{
    for (GridBlockWidget* c : m_gridArea->findChildren<GridBlockWidget*>())
        c->deleteLater();
    auto sl = m_ctx->routineService()->getSlotsForRoutine(m_routineId);
    for (const auto& s : sl)
        placeBlock(s.id, s.teacherId, s.teacherName, s.subject, s.day, s.startTime, s.endTime);
}

void RoutineGridWidget::placeBlock(int slotId, int teacherId, const QString& name, const QString& subject,
                                   const QString& day, const QTime& start, const QTime& end)
{
    int col = m_days.indexOf(day);
    if (col < 0) return;

    int startRow = m_start.secsTo(start) / 1800;
    int endRow   = m_start.secsTo(end) / 1800;
    int rowSpan  = qMax(1, endRow - startRow);

    crm::services::RoutineSlot s;
    s.id          = slotId;
    s.teacherId   = teacherId;
    s.teacherName = name;
    s.subject     = subject;
    s.startTime   = start;
    s.endTime     = end;

    GridBlockWidget* block = new GridBlockWidget(s, m_gridArea);
    block->setGeometry(
        kTimeW + col * kColW + 2,
        kHdrH + startRow * kRowH + 2,
        kColW - 4,
        rowSpan * kRowH - 4
    );
    block->show();

    connect(block, &GridBlockWidget::deleteRequested, this, [this](int slotId) {
        m_ctx->routineService()->deleteSlot(slotId);
        reload();
    });

    connect(block, &GridBlockWidget::resizeFinished, this,
            [this, day, start, teacherId](int slotId, int rowSpan) {
        QTime newEnd = start.addSecs(rowSpan * 1800);

        // ── conflict check on resize, excluding this slot itself ──
        auto conflicts = m_ctx->conflictService()->checkConflict(
            teacherId, day, start, newEnd, slotId);

        if (!conflicts.isEmpty()) {
            const auto& c = conflicts.first();
            QMessageBox::warning(this, "Teacher Conflict",
                QString("Resize would overlap existing booking on %1 from %2 to %3\n(%4 - %5)")
                    .arg(c.day,
                         c.startTime.toString("h:mm ap"),
                         c.endTime.toString("h:mm ap"),
                         c.program, c.section));
            reload();  // snap block back to its saved size
            return;
        }

        crm::services::RoutineSlot updated;
        updated.id        = slotId;
        updated.day       = day;
        updated.startTime = start;
        updated.endTime   = newEnd;
        m_ctx->routineService()->updateSlot(updated);
        reload();
    });
}