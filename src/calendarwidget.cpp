#include "calendarwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontMetrics>

const QString CalendarWidget::kDayNames[7] = {
    "PON", "UTO", "SRI", "ČET", "PET", "SUB", "NED"
};

CalendarWidget::CalendarWidget(EventManager *events, QWidget *parent)
    : QWidget(parent), m_events(events)
{
    m_today       = QDate::currentDate();
    m_selectedDate = m_today;
    setMonth(m_today.year(), m_today.month());
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    connect(m_events, &EventManager::eventsChanged,
            this, [this](const QDate&){ update(); });
}

void CalendarWidget::setMonth(int year, int month) {
    m_currentDate = QDate(year, month, 1);
    rebuildHolidays();
    update();
    emit monthChanged(year, month);
}

void CalendarWidget::rebuildHolidays() {
    int y = m_currentDate.year();
    m_holidays = OrthodoxHolidays::getHolidaysForYear(y);
    // Also get next year if December, prev if January
    if (m_currentDate.month() == 12) {
        auto next = OrthodoxHolidays::getHolidaysForYear(y+1);
        m_holidays.insert(next);
    }
    if (m_currentDate.month() == 1) {
        auto prev = OrthodoxHolidays::getHolidaysForYear(y-1);
        m_holidays.insert(prev);
    }
}

// ── Layout helpers ────────────────────────────────────────────────
QRectF CalendarWidget::cellRect(int row, int col) const {
    float w = (float)width();
    float cellW = w / 7.0f;
    float cellH = m_headerH;
    float gridTop = m_headerH + m_dayH;
    return QRectF(col * cellW, gridTop + row * cellH, cellW, cellH);
}

QDate CalendarWidget::dateAtPoint(const QPointF& pt) const {
    float gridTop = m_headerH + m_dayH;
    if (pt.y() < gridTop) return QDate();
    float cellW = (float)width() / 7.0f;
    float cellH = m_headerH;
    int col = (int)(pt.x() / cellW);
    int row = (int)((pt.y() - gridTop) / cellH);
    if (col < 0 || col > 6 || row < 0 || row >= m_gridRows) return QDate();

    // First cell of grid
    QDate first = m_currentDate;
    int dow = first.dayOfWeek(); // 1=Mon ... 7=Sun
    QDate startDate = first.addDays(-(dow - 1));
    return startDate.addDays(row * 7 + col);
}

// ── Events ───────────────────────────────────────────────────────
void CalendarWidget::mousePressEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d.isValid()) {
        m_selectedDate = d;
        update();
        emit dateClicked(d);
    }
}

void CalendarWidget::mouseDoubleClickEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d.isValid()) emit dateDoubleClicked(d);
}

void CalendarWidget::mouseMoveEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d != m_hoveredDate) { m_hoveredDate = d; update(); }
}

void CalendarWidget::leaveEvent(QEvent*) {
    m_hoveredDate = QDate(); update();
}

void CalendarWidget::resizeEvent(QResizeEvent *ev) {
    // Adjust cell height based on available space
    int available = ev->size().height() - (int)(m_headerH + m_dayH);
    m_headerH = (float)available / m_gridRows;
    if (m_headerH < 30) m_headerH = 30;
    update();
    QWidget::resizeEvent(ev);
}

// ── Painting ─────────────────────────────────────────────────────
void CalendarWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // Background
    p.fillRect(rect(), QColor(11, 11, 28));

    drawHeader(p);

    // Day name row
    float w = (float)width();
    float cellW = w / 7.0f;
    QFont dayFont;
    dayFont.setPixelSize(11);
    dayFont.setWeight(QFont::Medium);
    p.setFont(dayFont);
    for (int c = 0; c < 7; ++c) {
        QRectF r(c * cellW, m_headerH, cellW, m_dayH);
        // Subtle separator
        if (c > 0) {
            p.setPen(QPen(QColor(255,255,255,8), 1));
            p.drawLine(QPointF(r.left(), r.top()+4), QPointF(r.left(), r.bottom()-4));
        }
        QColor col = (c == 6) ? QColor("#e94560") : QColor("#555580");
        p.setPen(col);
        p.drawText(r, Qt::AlignCenter, kDayNames[c]);
    }

    // Horizontal separator
    p.setPen(QPen(QColor(255,255,255,12), 1));
    p.drawLine(0, (int)(m_headerH + m_dayH - 1), width(), (int)(m_headerH + m_dayH - 1));

    // Grid
    QDate first = m_currentDate;
    int dow = first.dayOfWeek();
    QDate startDate = first.addDays(-(dow - 1));

    for (int row = 0; row < m_gridRows; ++row) {
        for (int col = 0; col < 7; ++col) {
            QDate d = startDate.addDays(row * 7 + col);
            QRectF r = cellRect(row, col);
            bool isCurMonth = (d.month() == m_currentDate.month());
            bool isToday    = (d == m_today);
            bool isSel      = (d == m_selectedDate);
            bool isHov      = (d == m_hoveredDate);
            HolidayInfo hi  = m_holidays.value(d);
            bool hasEv      = m_events->hasEvents(d);
            drawDayCell(p, r, d, isCurMonth, isToday, isSel, isHov, hi, hasEv);
        }
    }
}

void CalendarWidget::drawHeader(QPainter& p) const {
    // Gradient header
    QLinearGradient hg(0, 0, width(), m_headerH);
    hg.setColorAt(0, QColor(30, 15, 60));
    hg.setColorAt(1, QColor(20, 10, 45));
    p.fillRect(QRectF(0, 0, width(), m_headerH), hg);
}

void CalendarWidget::drawDayCell(QPainter& p, const QRectF& r,
                                  const QDate& date,
                                  bool isCurMonth, bool isToday,
                                  bool isSel, bool isHov,
                                  const HolidayInfo& holiday,
                                  bool hasEvents) const
{
    bool isSunday  = (date.dayOfWeek() == 7);
    bool isHoliday = !holiday.name.isEmpty();
    bool isGreat   = holiday.isGreatFeast;

    // Cell background
    if (isToday) {
        // Today: golden glow
        QRadialGradient rg(r.center(), r.width() * 0.55f);
        rg.setColorAt(0, QColor(245, 166, 35, 55));
        rg.setColorAt(1, Qt::transparent);
        p.fillRect(r, rg);
        // Border
        p.setPen(QPen(QColor(245, 166, 35, 180), 1.5));
        QPainterPath bp;
        bp.addRoundedRect(r.adjusted(1,1,-1,-1), 8, 8);
        p.drawPath(bp);
    } else if (isSel) {
        QRadialGradient rg(r.center(), r.width() * 0.5f);
        rg.setColorAt(0, QColor(83, 52, 131, 60));
        rg.setColorAt(1, Qt::transparent);
        p.fillRect(r, rg);
        p.setPen(QPen(QColor(140, 100, 200, 140), 1));
        QPainterPath bp;
        bp.addRoundedRect(r.adjusted(1,1,-1,-1), 8, 8);
        p.drawPath(bp);
    } else if (isHov) {
        p.fillRect(r, QColor(255,255,255,8));
    }

    // Great feast: subtle red tint
    if (isGreat && isCurMonth) {
        p.fillRect(r, QColor(233, 69, 96, 12));
    }

    // Day number
    QFont numFont;
    int fontSize = qMax(11, (int)(r.height() * 0.38f));
    numFont.setPixelSize(qMin(fontSize, 18));
    numFont.setBold(isToday || isSel);
    p.setFont(numFont);

    QColor textColor;
    if (!isCurMonth) {
        textColor = QColor(60, 60, 90);
    } else if (isGreat) {
        textColor = QColor(233, 69, 96);   // Red for great feasts
    } else if (isHoliday) {
        textColor = QColor(200, 80, 100);  // Lighter red for minor feasts
    } else if (isSunday) {
        textColor = QColor(200, 60, 80);   // Sunday red
    } else if (isToday) {
        textColor = QColor(245, 200, 60);  // Gold for today
    } else {
        textColor = QColor(190, 190, 220);
    }
    p.setPen(textColor);

    // Number position: upper-center area of cell
    QRectF numRect(r.left(), r.top() + 4, r.width(), r.height() * 0.55f);
    p.drawText(numRect, Qt::AlignHCenter | Qt::AlignVCenter,
               QString::number(date.day()));

    // Holiday short name (small text)
    if (isHoliday && isCurMonth && r.height() > 44) {
        QFont hFont;
        hFont.setPixelSize(qMin(9, (int)(r.height() * 0.22f)));
        p.setFont(hFont);
        QColor hc = isGreat ? QColor(233, 69, 96, 200) : QColor(180, 80, 100, 180);
        p.setPen(hc);
        QRectF hRect(r.left()+1, r.top() + r.height()*0.58f,
                     r.width()-2, r.height()*0.3f);
        p.drawText(hRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
                   holiday.shortName);
    }

    // Event indicator dots
    if (hasEvents) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(83, 52, 200, 200));
        QPointF dotPos(r.center().x(), r.bottom() - 5);
        p.drawEllipse(dotPos, 3, 3);
    }

    // Vertical grid line
    if (date.dayOfWeek() < 7) {
        p.setPen(QPen(QColor(255,255,255,6), 1));
        p.drawLine(r.topRight(), r.bottomRight());
    }
    // Horizontal grid line
    p.setPen(QPen(QColor(255,255,255,6), 1));
    p.drawLine(r.bottomLeft(), r.bottomRight());
}
