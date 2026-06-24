#pragma once
#include <QWidget>
#include <QPainter>

class GridCanvas : public QWidget {
public:
    explicit GridCanvas(int rows, int cols,
                        int rowH, int colW,
                        int hdrH, int timeW,
                        QWidget* parent = nullptr)
        : QWidget(parent)
        , m_rows(rows), m_cols(cols)
        , m_rowH(rowH), m_colW(colW)
        , m_hdrH(hdrH), m_timeW(timeW)
    {
        setMinimumSize(timeW + cols * colW, hdrH + rows * rowH);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.fillRect(rect(), QColor("#0f172a"));

        QPen gridPen(QColor("#1e293b"));
        gridPen.setWidth(1);
        p.setPen(gridPen);

        for (int r = 0; r <= m_rows; ++r) {
            int y = m_hdrH + r * m_rowH;
            p.drawLine(m_timeW, y, m_timeW + m_cols * m_colW, y);
        }
        for (int c = 0; c <= m_cols; ++c) {
            int x = m_timeW + c * m_colW;
            p.drawLine(x, 0, x, m_hdrH + m_rows * m_rowH);
        }
    }

private:
    int m_rows, m_cols, m_rowH, m_colW, m_hdrH, m_timeW;
};