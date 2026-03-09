#include "calendarwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontMetrics>
#include <QtMath>

const QString CalendarWidget::kDayNames[7] = {
    "PON", "UTO", "SRI", "ČET", "PET", "SUB", "NED"
};

CalendarWidget::CalendarWidget(EventManager *events, QWidget *parent)
    : QWidget(parent), m_events(events)
{
    m_today        = QDate::currentDate();
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
    if (m_currentDate.month() == 12) { auto n = OrthodoxHolidays::getHolidaysForYear(y+1); m_holidays.insert(n); }
    if (m_currentDate.month() == 1)  { auto p = OrthodoxHolidays::getHolidaysForYear(y-1); m_holidays.insert(p); }
}

QRectF CalendarWidget::cellRect(int row, int col) const {
    float cellW = (float)width() / 7.0f;
    float gridTop = m_headerH + m_dayH;
    return QRectF(col * cellW, gridTop + row * cellH, cellW, cellH);
}

QDate CalendarWidget::dateAtPoint(const QPointF& pt) const {
    float gridTop = m_headerH + m_dayH;
    if (pt.y() < gridTop) return QDate();
    float cellW = (float)width() / 7.0f;
    int col = (int)(pt.x() / cellW);
    int row = (int)((pt.y() - gridTop) / cellH);
    if (col < 0 || col > 6 || row < 0 || row >= m_gridRows) return QDate();
    QDate first = m_currentDate;
    int dow = first.dayOfWeek();
    return first.addDays(-(dow-1) + row*7 + col);
}

void CalendarWidget::mousePressEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d.isValid()) { m_selectedDate = d; update(); emit dateClicked(d); }
}

void CalendarWidget::mouseDoubleClickEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d.isValid()) emit dateDoubleClicked(d);
}

void CalendarWidget::mouseMoveEvent(QMouseEvent *ev) {
    QDate d = dateAtPoint(ev->position());
    if (d != m_hoveredDate) { m_hoveredDate = d; update(); }
}

void CalendarWidget::leaveEvent(QEvent*) { m_hoveredDate = QDate(); update(); }

void CalendarWidget::resizeEvent(QResizeEvent *ev) {
    int available = ev->size().height() - (int)(m_headerH + m_dayH);
    cellH = qMax(30.0f, (float)available / m_gridRows);
    update();
    QWidget::resizeEvent(ev);
}

// ── Seasonal background painter ──────────────────────────────────
static void drawSeasonalBackground(QPainter& p, const QRect& r, int month) {
    p.save();
    p.setRenderHint(QPainter::Antialiasing);

    // Season: Dec-Feb=Zima, Mar-May=Proljece, Jun-Aug=Ljeto, Sep-Nov=Jesen
    if (month == 12 || month == 1 || month == 2) {
        // ── ZIMA: Duboka plava noć, zvijezde, snijeg ──
        QLinearGradient bg(0, 0, 0, r.height());
        bg.setColorAt(0.0, QColor(5,  8,  28));
        bg.setColorAt(0.5, QColor(8,  14, 40));
        bg.setColorAt(1.0, QColor(12, 20, 50));
        p.fillRect(r, bg);

        // Stars
        p.setPen(Qt::NoPen);
        QList<QPointF> stars = {
            {0.05f,0.04f},{0.12f,0.08f},{0.22f,0.03f},{0.35f,0.07f},{0.48f,0.02f},
            {0.58f,0.06f},{0.70f,0.04f},{0.82f,0.09f},{0.92f,0.03f},{0.97f,0.07f},
            {0.15f,0.14f},{0.40f,0.12f},{0.65f,0.15f},{0.88f,0.13f},{0.28f,0.18f},
            {0.75f,0.20f},{0.55f,0.22f},{0.08f,0.22f},{0.95f,0.18f},{0.45f,0.25f}
        };
        for (auto& s : stars) {
            float sz = 1.2f + (qSin(s.x()*17.3f)*0.5f+0.5f)*1.5f;
            p.setBrush(QColor(220, 230, 255, 180 + (int)(50*qSin(s.y()*13))));
            p.drawEllipse(QPointF(r.left()+s.x()*r.width(), r.top()+s.y()*r.height()*0.6f), sz, sz);
        }
        // Moon glow
        QRadialGradient moon(r.left()+r.width()*0.82f, r.top()+r.height()*0.12f, 40);
        moon.setColorAt(0, QColor(230,240,255,60));
        moon.setColorAt(1, Qt::transparent);
        p.fillRect(r, moon);
        // Snow hills
        QLinearGradient snow(0, r.height()*0.65f, 0, r.height());
        snow.setColorAt(0, QColor(180,200,230,80));
        snow.setColorAt(1, QColor(200,220,250,120));
        QPainterPath hill;
        hill.moveTo(r.left(), r.bottom());
        hill.cubicTo(r.left()+r.width()*0.2f, r.top()+r.height()*0.60f,
                     r.left()+r.width()*0.45f, r.top()+r.height()*0.55f,
                     r.left()+r.width()*0.5f,  r.top()+r.height()*0.62f);
        hill.cubicTo(r.left()+r.width()*0.6f, r.top()+r.height()*0.50f,
                     r.left()+r.width()*0.80f, r.top()+r.height()*0.58f,
                     r.right(), r.top()+r.height()*0.65f);
        hill.lineTo(r.right(), r.bottom());
        hill.closeSubpath();
        p.fillPath(hill, snow);
        // Snowflakes
        p.setPen(QPen(QColor(255,255,255,60), 0.8f));
        for (int i = 0; i < 18; ++i) {
            float x = r.left() + (i*137.5f / 18.0f) * r.width() / 137.5f * 137.5f;
            x = r.left() + ((i * 73) % 100) * r.width() / 100.0f;
            float y = r.top() + ((i * 47 + 10) % 80) * r.height() * 0.7f / 80.0f;
            float sz = 2.5f + (i%3)*1.5f;
            p.drawLine(QPointF(x-sz,y), QPointF(x+sz,y));
            p.drawLine(QPointF(x,y-sz), QPointF(x,y+sz));
        }

    } else if (month >= 3 && month <= 5) {
        // ── PROLJECE: Zelena livada, cvijeće, sunce ──
        QLinearGradient bg(0, 0, 0, r.height());
        bg.setColorAt(0.0, QColor(100, 170, 235));
        bg.setColorAt(0.45f, QColor(140, 200, 245));
        bg.setColorAt(0.46f, QColor(80,  160, 60));
        bg.setColorAt(1.0, QColor(50,  130, 40));
        p.fillRect(r, bg);
        // Sun
        QRadialGradient sun(r.left()+r.width()*0.15f, r.top()+r.height()*0.18f, 35);
        sun.setColorAt(0, QColor(255,240,100,200));
        sun.setColorAt(0.4f, QColor(255,200,50,100));
        sun.setColorAt(1, Qt::transparent);
        p.fillRect(r, sun);
        // Clouds
        auto drawCloud = [&](float cx, float cy, float scale) {
            p.setBrush(QColor(255,255,255,120));
            p.setPen(Qt::NoPen);
            float bx = r.left()+cx*r.width(), by = r.top()+cy*r.height();
            p.drawEllipse(QPointF(bx, by), 20*scale, 13*scale);
            p.drawEllipse(QPointF(bx+18*scale, by-5*scale), 16*scale, 11*scale);
            p.drawEllipse(QPointF(bx-16*scale, by-3*scale), 14*scale, 10*scale);
        };
        drawCloud(0.55f, 0.10f, 1.0f);
        drawCloud(0.80f, 0.16f, 0.7f);
        // Flowers
        auto drawFlower = [&](float fx, float fy, QColor col) {
            float x = r.left()+fx*r.width(), y = r.top()+fy*r.height();
            p.setPen(Qt::NoPen);
            for (int i = 0; i < 5; ++i) {
                float a = i * 72.0f * M_PI / 180.0f;
                p.setBrush(col);
                p.drawEllipse(QPointF(x+qCos(a)*5, y+qSin(a)*5), 3, 3);
            }
            p.setBrush(QColor(255,230,50));
            p.drawEllipse(QPointF(x,y), 2.5f, 2.5f);
        };
        drawFlower(0.08f, 0.80f, QColor(255,80,120));
        drawFlower(0.20f, 0.85f, QColor(200,100,255));
        drawFlower(0.35f, 0.78f, QColor(255,200,50));
        drawFlower(0.55f, 0.82f, QColor(255,80,120));
        drawFlower(0.72f, 0.75f, QColor(100,200,255));
        drawFlower(0.88f, 0.80f, QColor(255,150,50));
        // Grass blades
        p.setPen(QPen(QColor(40,120,30,120), 1.2f));
        for (int i = 0; i < 30; ++i) {
            float x = r.left() + (i*33%100) * r.width()/100.0f;
            float y = r.top() + r.height()*0.73f;
            float h = 6+i%8;
            p.drawLine(QPointF(x,y), QPointF(x+(i%3-1)*3, y-h));
        }

    } else if (month >= 6 && month <= 8) {
        // ── LJETO: Plavo nebo, more/jezero, sunce ──
        QLinearGradient bg(0, 0, 0, r.height());
        bg.setColorAt(0.0, QColor(30,  120, 210));
        bg.setColorAt(0.40f, QColor(80,  170, 230));
        bg.setColorAt(0.41f, QColor(20,  100, 160));
        bg.setColorAt(0.70f, QColor(15,  80,  140));
        bg.setColorAt(1.0, QColor(10,  60,  110));
        p.fillRect(r, bg);
        // Blazing sun
        QRadialGradient sun(r.left()+r.width()*0.78f, r.top()+r.height()*0.12f, 50);
        sun.setColorAt(0, QColor(255,240,100,230));
        sun.setColorAt(0.3f, QColor(255,180,30,150));
        sun.setColorAt(0.6f, QColor(255,150,0,60));
        sun.setColorAt(1, Qt::transparent);
        p.fillRect(r, sun);
        // Sun rays
        p.setPen(QPen(QColor(255,220,80,40), 1.5f));
        float sx = r.left()+r.width()*0.78f, sy = r.top()+r.height()*0.12f;
        for (int i = 0; i < 12; ++i) {
            float a = i * 30.0f * M_PI / 180.0f;
            p.drawLine(QPointF(sx+qCos(a)*22, sy+qSin(a)*22),
                       QPointF(sx+qCos(a)*40, sy+qSin(a)*40));
        }
        // Waves on water
        p.setPen(QPen(QColor(255,255,255,50), 1.0f));
        float wy = r.top()+r.height()*0.48f;
        for (int row2 = 0; row2 < 5; ++row2) {
            float y2 = wy + row2*6;
            QPainterPath wave;
            wave.moveTo(r.left(), y2);
            for (int wx2 = 0; wx2 < r.width(); wx2 += 20) {
                wave.quadTo(r.left()+wx2+5, y2-3, r.left()+wx2+10, y2);
                wave.quadTo(r.left()+wx2+15, y2+3, r.left()+wx2+20, y2);
            }
            p.drawPath(wave);
        }
        // Beach/shore strip
        QLinearGradient beach(0, r.height()*0.82f, 0, r.height());
        beach.setColorAt(0, QColor(240,210,140,100));
        beach.setColorAt(1, QColor(220,190,110,150));
        p.fillRect(QRectF(r.left(), r.top()+r.height()*0.82f, r.width(), r.height()*0.18f), beach);

    } else {
        // ── JESEN: Zlatno-narandžasta šuma, magla ──
        QLinearGradient bg(0, 0, 0, r.height());
        bg.setColorAt(0.0, QColor(60,  45,  30));
        bg.setColorAt(0.3f, QColor(90,  60,  35));
        bg.setColorAt(0.5f, QColor(140, 90,  40));
        bg.setColorAt(0.6f, QColor(60,  45,  20));
        bg.setColorAt(1.0, QColor(40,  30,  15));
        p.fillRect(r, bg);
        // Misty sky gradient
        QLinearGradient mist(0, 0, 0, r.height()*0.55f);
        mist.setColorAt(0, QColor(180,140,80,80));
        mist.setColorAt(1, Qt::transparent);
        p.fillRect(r, mist);
        // Autumn trees (silhouettes)
        auto drawTree = [&](float tx, float ty, float scale, QColor col) {
            float bx = r.left()+tx*r.width(), by = r.top()+ty*r.height();
            // trunk
            p.setBrush(QColor(40,25,10,180));
            p.setPen(Qt::NoPen);
            p.drawRect(QRectF(bx-3*scale, by, 6*scale, 20*scale));
            // canopy layers
            for (int layer = 0; layer < 3; ++layer) {
                QColor lc = col;
                lc.setAlpha(120 + layer*30);
                p.setBrush(lc);
                float ly = by - (layer+1)*16*scale;
                float lw = (3-layer)*14*scale;
                QPainterPath canopy;
                canopy.addEllipse(QPointF(bx, ly), lw, lw*0.7f);
                p.fillPath(canopy, lc);
            }
        };
        drawTree(0.05f, 0.65f, 1.1f, QColor(200,80,20,160));
        drawTree(0.18f, 0.60f, 0.9f, QColor(220,120,20,160));
        drawTree(0.30f, 0.68f, 1.2f, QColor(180,60,10,160));
        drawTree(0.55f, 0.58f, 0.8f, QColor(230,150,30,160));
        drawTree(0.70f, 0.63f, 1.0f, QColor(200,90,15,160));
        drawTree(0.85f, 0.60f, 1.1f, QColor(210,110,25,160));
        drawTree(0.95f, 0.67f, 0.8f, QColor(190,70,10,160));
        // Falling leaves
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 20; ++i) {
            float lx = r.left() + ((i*73+5)%100)*r.width()/100.0f;
            float ly = r.top()  + ((i*47+8)%70)*r.height()*0.7f/100.0f;
            float angle = (i*37)%60 - 30;
            p.save();
            p.translate(lx, ly);
            p.rotate(angle);
            QColor leafCol = (i%3==0) ? QColor(220,80,20,180)
                           : (i%3==1) ? QColor(200,130,20,160)
                                      : QColor(180,50,10,170);
            p.setBrush(leafCol);
            QPainterPath leaf;
            leaf.moveTo(0, -4);
            leaf.cubicTo(3,-2, 3,2, 0,4);
            leaf.cubicTo(-3,2,-3,-2, 0,-4);
            p.fillPath(leaf, leafCol);
            p.restore();
        }
    }

    // Subtle dark overlay for readability on top (header area)
    QLinearGradient overlay(0, 0, 0, r.height()*0.35f);
    overlay.setColorAt(0, QColor(0,0,0,100));
    overlay.setColorAt(1, Qt::transparent);
    p.fillRect(r, overlay);

    p.restore();
}

// ── paintEvent ───────────────────────────────────────────────────
void CalendarWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    int month = m_currentDate.month();

    // Draw seasonal background
    drawSeasonalBackground(p, rect(), month);

    // Day names header
    float w = (float)width();
    float cellW = w / 7.0f;
    QFont dayFont;
    dayFont.setPixelSize(11);
    dayFont.setWeight(QFont::Medium);
    p.setFont(dayFont);
    for (int c = 0; c < 7; ++c) {
        QRectF r2(c * cellW, m_headerH, cellW, m_dayH);
        QColor col = (c == 6) ? QColor("#e94560") : QColor(200,200,220,200);
        p.setPen(col);
        p.drawText(r2, Qt::AlignCenter, kDayNames[c]);
    }

    // Subtle separator line
    p.setPen(QPen(QColor(255,255,255,25), 1));
    p.drawLine(0, (int)(m_headerH + m_dayH - 1), width(), (int)(m_headerH + m_dayH - 1));

    // Grid cells
    QDate first = m_currentDate;
    int dow = first.dayOfWeek();
    QDate startDate = first.addDays(-(dow - 1));

    for (int row = 0; row < m_gridRows; ++row) {
        for (int col = 0; col < 7; ++col) {
            QDate d = startDate.addDays(row * 7 + col);
            QRectF r3 = cellRect(row, col);
            bool isCurMonth = (d.month() == m_currentDate.month());
            bool isToday    = (d == m_today);
            bool isSel      = (d == m_selectedDate);
            bool isHov      = (d == m_hoveredDate);
            HolidayInfo hi  = m_holidays.value(d);
            bool hasEv      = m_events->hasEvents(d);
            drawDayCell(p, r3, d, isCurMonth, isToday, isSel, isHov, hi, hasEv);
        }
    }
}

void CalendarWidget::drawDayCell(QPainter& p, const QRectF& r,
                                  const QDate& date,
                                  bool isCurMonth, bool isToday,
                                  bool isSel, bool isHov,
                                  const HolidayInfo& holiday,
                                  bool hasEvents) const
{
    bool isSunday  = (date.dayOfWeek() == 7);
    bool isGreat   = holiday.isGreatFeast;
    bool isHoliday = !holiday.name.isEmpty();

    // Cell background
    if (isSel) {
        p.setBrush(QColor(83,52,131,90));
        p.setPen(QPen(QColor(140,100,200,160), 1.5));
        QPainterPath bp; bp.addRoundedRect(r.adjusted(1,1,-1,-1), 8, 8);
        p.fillPath(bp, QColor(83,52,131,90));
        p.drawPath(bp);
    } else if (isToday) {
        QRadialGradient rg(r.center(), r.width()*0.55f);
        rg.setColorAt(0, QColor(245,166,35,70));
        rg.setColorAt(1, Qt::transparent);
        p.fillRect(r, rg);
        p.setPen(QPen(QColor(245,166,35,200), 1.5));
        QPainterPath bp; bp.addRoundedRect(r.adjusted(1,1,-1,-1), 8, 8);
        p.drawPath(bp);
    } else if (isHov) {
        p.fillRect(r, QColor(255,255,255,18));
    }

    // Red tint for great feasts
    if (isGreat && isCurMonth) {
        p.fillRect(r, QColor(233,69,96,18));
    }

    // Day number
    QFont numFont;
    int fontSize = qMax(11, (int)(cellH * 0.38f));
    numFont.setPixelSize(qMin(fontSize, 20));
    numFont.setBold(isToday || isSel);
    p.setFont(numFont);

    QColor textColor;
    if (!isCurMonth)      textColor = QColor(80,80,110,140);
    else if (isGreat)     textColor = QColor(255,100,120);
    else if (isHoliday)   textColor = QColor(220,100,110);
    else if (isSunday)    textColor = QColor(220,80,90);
    else if (isToday)     textColor = QColor(255,210,80);
    else                  textColor = QColor(220,220,240);

    p.setPen(textColor);
    QRectF numRect(r.left(), r.top()+3, r.width(), r.height()*0.52f);
    p.drawText(numRect, Qt::AlignHCenter|Qt::AlignVCenter, QString::number(date.day()));

    // Holiday short name
    if (isHoliday && isCurMonth && cellH > 44) {
        QFont hFont;
        hFont.setPixelSize(qMin(9, (int)(cellH*0.22f)));
        p.setFont(hFont);
        QColor hc = isGreat ? QColor(255,110,130,220) : QColor(200,100,110,190);
        p.setPen(hc);
        QRectF hRect(r.left()+1, r.top()+r.height()*0.56f, r.width()-2, r.height()*0.32f);
        p.drawText(hRect, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, holiday.shortName);
    }

    // Event dot
    if (hasEvents) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(100,80,220,210));
        p.drawEllipse(QPointF(r.center().x(), r.bottom()-5), 3, 3);
    }

    // Grid lines
    if (date.dayOfWeek() < 7) {
        p.setPen(QPen(QColor(255,255,255,10), 1));
        p.drawLine(r.topRight(), r.bottomRight());
    }
    p.setPen(QPen(QColor(255,255,255,10), 1));
    p.drawLine(r.bottomLeft(), r.bottomRight());
}
