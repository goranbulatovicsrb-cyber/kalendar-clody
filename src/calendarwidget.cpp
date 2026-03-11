#include "calendarwidget.h"
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QSettings>
#include <QFile>
#include <QtSvg/QSvgRenderer>
#include <QMessageBox>
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

const QString CalendarWidget::kSeasonNames[4] = {
    "Zima \u2744 (Dec, Jan, Feb)",
    "Prolje\u0107e \u1F338 (Mar, Apr, Maj)",
    "Ljeto \u2600 (Jun, Jul, Aug)",
    "Jesen \U0001F342 (Sep, Okt, Nov)"
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
    loadSeasonSettings();
}

// ── Season helpers ─────────────────────────────────────────────
int CalendarWidget::currentSeason() const {
    int m = m_currentDate.month();
    if (m == 12 || m == 1 || m == 2) return 0; // Zima
    if (m >= 3 && m <= 5)            return 1; // Proljece
    if (m >= 6 && m <= 8)            return 2; // Ljeto
    return 3;                                   // Jesen
}

void CalendarWidget::setSeasonImage(int season, const QString& path) {
    if (season < 0 || season > 3) return;
    m_seasonPaths[season] = path;
    if (path.isEmpty()) {
        m_seasonPixmaps[season] = QPixmap();
    } else {
        m_seasonPixmaps[season] = QPixmap(path);
    }
    saveSeasonSettings();
    update();
    emit seasonImageChanged(season);
}

void CalendarWidget::loadSeasonSettings() {
    // Load built-in SVG defaults first
    const QString svgRes[4] = {
        ":/season_zima.svg", ":/season_proljece.svg",
        ":/season_ljeto.svg", ":/season_jesen.svg"
    };
    for (int i = 0; i < 4; ++i) {
        QPixmap px(800, 600);
        px.fill(Qt::transparent);
        QPainter painter(&px);
        QSvgRenderer renderer(svgRes[i]);
        if (renderer.isValid()) {
            renderer.render(&painter);
            m_seasonPixmaps[i] = px;
        }
        painter.end();
    }
    // Override with user custom images if saved
    QSettings s("OrthodoxCalendar", "PravoslavniKalendar");
    const QString keys[4] = {"season/zima","season/proljece","season/ljeto","season/jesen"};
    for (int i = 0; i < 4; ++i) {
        QString path = s.value(keys[i], "").toString();
        if (!path.isEmpty() && QFile::exists(path)) {
            m_seasonPaths[i]   = path;
            m_seasonPixmaps[i] = QPixmap(path);
        }
    }
}

void CalendarWidget::saveSeasonSettings() {
    QSettings s("OrthodoxCalendar", "PravoslavniKalendar");
    const QString keys[4] = {"season/zima","season/proljece","season/ljeto","season/jesen"};
    for (int i = 0; i < 4; ++i)
        s.setValue(keys[i], m_seasonPaths[i]);
}

void CalendarWidget::contextMenuEvent(QContextMenuEvent *event) {
    int season = currentSeason();
    const QString icons[4] = {"❄️","🌸","☀️","🍂"};
    const QString names[4] = {"Zima","Proljeće","Ljeto","Jesen"};

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background:#12122e; color:#e0e0f0; border:1px solid rgba(153,102,204,0.4); border-radius:8px; padding:4px; }"
        "QMenu::item { padding:8px 20px; border-radius:5px; font-size:13px; }"
        "QMenu::item:selected { background:rgba(153,102,204,0.3); color:#ffffff; }"
        "QMenu::separator { background:rgba(255,255,255,0.1); height:1px; margin:4px 10px; }");

    QAction *title = menu.addAction(icons[season] + "  Pozadina — " + names[season]);
    title->setEnabled(false);
    menu.addSeparator();

    QAction *actChange = menu.addAction("🖼  Odaberi sliku...");
    QAction *actRemove = menu.addAction("✖  Ukloni sliku (vrati crtanu)");
    actRemove->setEnabled(!m_seasonPaths[season].isEmpty());

    menu.addSeparator();
    // Quick change for other seasons
    QMenu *otherMenu = menu.addMenu("🗓  Promijeni drugu sezonu");
    otherMenu->setStyleSheet(menu.styleSheet());
    for (int i = 0; i < 4; ++i) {
        if (i == season) continue;
        QAction *a = otherMenu->addAction(icons[i] + "  " + names[i]);
        connect(a, &QAction::triggered, this, [this, i]() {
            QString path = QFileDialog::getOpenFileName(this,
                "Odaberi sliku za " + kSeasonNames[i], "",
                "Slike (*.png *.jpg *.jpeg *.bmp *.webp)");
            if (!path.isEmpty()) setSeasonImage(i, path);
        });
    }

    connect(actChange, &QAction::triggered, this, [this, season]() {
        QString path = QFileDialog::getOpenFileName(this,
            "Odaberi sliku za " + kSeasonNames[season], "",
            "Slike (*.png *.jpg *.jpeg *.bmp *.webp)");
        if (!path.isEmpty()) setSeasonImage(season, path);
    });
    connect(actRemove, &QAction::triggered, this, [this, season]() {
        setSeasonImage(season, "");
    });

    menu.exec(event->globalPos());
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
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const float W = r.width(), H = r.height();
    const float L = r.left(), T = r.top();

    // ════════════════════════════════════════════════════════
    // ❄️  ZIMA  (Dec, Jan, Feb)
    // ════════════════════════════════════════════════════════
    if (month == 12 || month == 1 || month == 2) {

        // Sky: deep midnight blue → indigo
        QLinearGradient sky(0, 0, 0, H);
        sky.setColorAt(0.00f, QColor(2,   4,  22));
        sky.setColorAt(0.35f, QColor(6,  12,  45));
        sky.setColorAt(0.65f, QColor(10, 20,  60));
        sky.setColorAt(1.00f, QColor(18, 30,  75));
        p.fillRect(r, sky);

        // Milky way band (diagonal glow)
        QLinearGradient milky(L, T, L+W*0.7f, T+H*0.5f);
        milky.setColorAt(0,    Qt::transparent);
        milky.setColorAt(0.4f, QColor(100,120,200, 18));
        milky.setColorAt(0.6f, QColor(120,140,220, 25));
        milky.setColorAt(1,    Qt::transparent);
        p.fillRect(r, milky);

        // Stars (many, varied sizes)
        p.setPen(Qt::NoPen);
        struct Star { float x,y,s; int a; };
        Star stars[] = {
            {0.03f,0.02f,1.8f,220},{0.09f,0.06f,1.2f,180},{0.15f,0.03f,2.2f,240},
            {0.22f,0.08f,1.5f,200},{0.29f,0.02f,1.0f,160},{0.38f,0.05f,2.5f,255},
            {0.45f,0.09f,1.3f,190},{0.52f,0.03f,1.8f,220},{0.61f,0.07f,1.1f,170},
            {0.68f,0.02f,2.0f,240},{0.76f,0.06f,1.5f,200},{0.84f,0.04f,2.3f,230},
            {0.91f,0.08f,1.0f,160},{0.96f,0.03f,1.7f,210},{0.12f,0.14f,1.4f,190},
            {0.25f,0.17f,2.8f,255},{0.42f,0.13f,1.2f,180},{0.57f,0.18f,1.6f,210},
            {0.73f,0.15f,2.0f,235},{0.88f,0.12f,1.3f,195},{0.34f,0.22f,1.5f,200},
            {0.50f,0.25f,1.8f,220},{0.65f,0.20f,1.1f,175},{0.80f,0.23f,2.1f,240},
            {0.07f,0.26f,1.4f,190},{0.18f,0.30f,1.0f,165},{0.93f,0.22f,1.6f,215},
        };
        for (auto& s : stars) {
            // Twinkle: slight blue-white variation
            int blue = 200 + (int)(s.x * 55);
            p.setBrush(QColor(200, 215, blue, s.a));
            p.drawEllipse(QPointF(L+s.x*W, T+s.y*H*0.75f), s.s, s.s);
            // Glow around bright stars
            if (s.s > 2.0f) {
                QRadialGradient glow(L+s.x*W, T+s.y*H*0.75f, s.s*4);
                glow.setColorAt(0, QColor(180,200,255,40));
                glow.setColorAt(1, Qt::transparent);
                p.fillRect(QRectF(L+s.x*W-s.s*4, T+s.y*H*0.75f-s.s*4, s.s*8, s.s*8), glow);
            }
        }

        // Full moon — large, luminous
        float mx = L+W*0.80f, my = T+H*0.13f;
        // Outer atmosphere glow
        QRadialGradient moonAtm(mx, my, 70);
        moonAtm.setColorAt(0,    QColor(200,220,255, 25));
        moonAtm.setColorAt(0.5f, QColor(180,200,240, 15));
        moonAtm.setColorAt(1,    Qt::transparent);
        p.fillRect(QRectF(mx-70,my-70,140,140), moonAtm);
        // Moon body
        QRadialGradient moonBody(mx-5, my-5, 24);
        moonBody.setColorAt(0,    QColor(240,245,255,240));
        moonBody.setColorAt(0.6f, QColor(210,225,250,230));
        moonBody.setColorAt(1,    QColor(180,200,240,200));
        p.setBrush(moonBody);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(mx, my), 22, 22);
        // Moon craters (subtle)
        p.setBrush(QColor(160,180,220,30));
        p.drawEllipse(QPointF(mx+6, my-4), 5, 5);
        p.drawEllipse(QPointF(mx-8, my+6), 4, 4);
        p.drawEllipse(QPointF(mx+2, my+9), 3, 3);

        // Snow-covered mountain silhouettes (background, dark blue)
        p.setPen(Qt::NoPen);
        auto drawMountain = [&](float px, float pw, float ph, QColor col) {
            QPainterPath m;
            m.moveTo(L+px*W-pw*W*0.5f, T+H);
            m.lineTo(L+px*W, T+H*(1.0f-ph));
            m.lineTo(L+px*W+pw*W*0.5f, T+H);
            p.fillPath(m, col);
            // Snow cap
            QPainterPath cap;
            float capH = ph * 0.22f;
            cap.moveTo(L+px*W-pw*W*0.12f, T+H*(1.0f-ph+capH));
            cap.lineTo(L+px*W, T+H*(1.0f-ph));
            cap.lineTo(L+px*W+pw*W*0.12f, T+H*(1.0f-ph+capH));
            p.fillPath(cap, QColor(230,240,255,200));
        };
        drawMountain(0.15f, 0.30f, 0.38f, QColor(15,25,60,200));
        drawMountain(0.40f, 0.35f, 0.45f, QColor(12,20,52,220));
        drawMountain(0.68f, 0.28f, 0.35f, QColor(18,28,65,190));
        drawMountain(0.88f, 0.25f, 0.32f, QColor(14,22,58,200));

        // Frozen lake reflection at bottom
        float lakeY = T+H*0.72f;
        QLinearGradient lake(0, lakeY, 0, T+H);
        lake.setColorAt(0,    QColor(20,35,80, 180));
        lake.setColorAt(0.3f, QColor(15,28,65, 200));
        lake.setColorAt(1.0f, QColor(10,20,50, 220));
        p.fillRect(QRectF(L, lakeY, W, H-(lakeY-T)), lake);
        // Ice cracks on lake
        p.setPen(QPen(QColor(140,170,220,40), 0.7f));
        QPainterPath crack1; crack1.moveTo(L+W*0.2f, lakeY+5);
        crack1.lineTo(L+W*0.3f, lakeY+18); crack1.lineTo(L+W*0.25f, lakeY+35);
        QPainterPath crack2; crack2.moveTo(L+W*0.6f, lakeY+8);
        crack2.lineTo(L+W*0.7f, lakeY+22); crack2.lineTo(L+W*0.65f, lakeY+40);
        p.drawPath(crack1); p.drawPath(crack2);
        // Moon reflection in lake
        QRadialGradient moonRefl(mx, lakeY+(T+H-lakeY)*0.3f, 15);
        moonRefl.setColorAt(0,    QColor(180,200,240,60));
        moonRefl.setColorAt(1,    Qt::transparent);
        p.fillRect(QRectF(mx-20, lakeY, 40, 40), moonRefl);

        // Snow hills (foreground, light blue-white)
        QPainterPath hill1;
        hill1.moveTo(L, T+H);
        hill1.cubicTo(L+W*0.1f, T+H*0.70f, L+W*0.28f, T+H*0.65f, L+W*0.38f, T+H*0.72f);
        hill1.cubicTo(L+W*0.50f, T+H*0.78f, L+W*0.60f, T+H*0.68f, L+W*0.72f, T+H*0.73f);
        hill1.cubicTo(L+W*0.85f, T+H*0.78f, L+W*0.95f, T+H*0.72f, L+W, T+H*0.75f);
        hill1.lineTo(L+W, T+H); hill1.closeSubpath();
        QLinearGradient snowGrad(0, T+H*0.65f, 0, T+H);
        snowGrad.setColorAt(0,    QColor(210,228,255,200));
        snowGrad.setColorAt(0.4f, QColor(190,215,250,210));
        snowGrad.setColorAt(1.0f, QColor(170,200,240,220));
        p.fillPath(hill1, snowGrad);
        // Snow shadow depth
        QPainterPath hillShadow;
        hillShadow.moveTo(L, T+H);
        hillShadow.cubicTo(L+W*0.1f, T+H*0.72f, L+W*0.28f, T+H*0.68f, L+W*0.38f, T+H*0.74f);
        hillShadow.lineTo(L, T+H); hillShadow.closeSubpath();
        p.fillPath(hillShadow, QColor(100,130,200,40));

        // Snowflakes — decorative, varied sizes
        p.setPen(Qt::NoPen);
        struct Flake { float x,y,s; int a; };
        Flake flakes[] = {
            {0.05f,0.35f,3.5f,120},{0.14f,0.48f,2.0f,90},{0.23f,0.38f,4.5f,110},
            {0.31f,0.52f,2.5f,100},{0.42f,0.40f,3.0f,115},{0.51f,0.55f,2.0f,85},
            {0.60f,0.42f,4.0f,120},{0.70f,0.50f,2.5f,95},{0.79f,0.36f,3.5f,110},
            {0.88f,0.46f,2.0f,90},{0.95f,0.38f,3.0f,105},
        };
        for (auto& f : flakes) {
            float fx = L+f.x*W, fy = T+f.y*H;
            // 6-pointed snowflake
            p.setPen(QPen(QColor(200,220,255,f.a), 0.9f));
            for (int i = 0; i < 6; ++i) {
                float a = i * 60.0f * M_PI / 180.0f;
                p.drawLine(QPointF(fx, fy), QPointF(fx+qCos(a)*f.s, fy+qSin(a)*f.s));
                // tiny branches
                float bx = fx+qCos(a)*f.s*0.6f, by = fy+qSin(a)*f.s*0.6f;
                float ba = a + 45.0f*M_PI/180.0f;
                p.drawLine(QPointF(bx,by), QPointF(bx+qCos(ba)*f.s*0.3f, by+qSin(ba)*f.s*0.3f));
            }
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(220,235,255,f.a));
            p.drawEllipse(QPointF(fx,fy), 0.8f, 0.8f);
        }

        // Aurora borealis — faint green/teal curtains
        for (int i = 0; i < 3; ++i) {
            float ax = L + (0.1f + i*0.3f)*W;
            float aw = W * 0.25f;
            QLinearGradient aurora(ax, T+H*0.1f, ax, T+H*0.45f);
            QColor auroraCol = (i%2==0) ? QColor(30,180,100,30) : QColor(20,150,180,25);
            aurora.setColorAt(0, auroraCol);
            aurora.setColorAt(0.6f, QColor(auroraCol.red(),auroraCol.green(),auroraCol.blue(),15));
            aurora.setColorAt(1, Qt::transparent);
            QPainterPath aPath;
            aPath.moveTo(ax, T+H*0.1f);
            aPath.cubicTo(ax+aw*0.3f, T+H*0.2f, ax+aw*0.7f, T+H*0.15f, ax+aw, T+H*0.12f);
            aPath.lineTo(ax+aw, T+H*0.45f);
            aPath.cubicTo(ax+aw*0.7f, T+H*0.42f, ax+aw*0.3f, T+H*0.48f, ax, T+H*0.45f);
            aPath.closeSubpath();
            p.fillPath(aPath, aurora);
        }

    // ════════════════════════════════════════════════════════
    // 🌸  PROLJEĆE  (Mar, Apr, May)
    // ════════════════════════════════════════════════════════
    } else if (month >= 3 && month <= 5) {

        // Sky: warm morning blue
        QLinearGradient sky(0, 0, 0, H);
        sky.setColorAt(0.00f, QColor(90,  160, 235));
        sky.setColorAt(0.30f, QColor(135, 195, 250));
        sky.setColorAt(0.55f, QColor(170, 215, 255));
        sky.setColorAt(0.56f, QColor(90,  170,  80));
        sky.setColorAt(0.75f, QColor(65,  145,  55));
        sky.setColorAt(1.00f, QColor(45,  120,  40));
        p.fillRect(r, sky);

        // Sunrise glow on horizon
        QRadialGradient sunrise(L+W*0.5f, T+H*0.56f, W*0.55f);
        sunrise.setColorAt(0,    QColor(255,230,150, 80));
        sunrise.setColorAt(0.3f, QColor(255,200,100, 40));
        sunrise.setColorAt(0.7f, QColor(255,160, 60, 15));
        sunrise.setColorAt(1,    Qt::transparent);
        p.fillRect(r, sunrise);

        // Sun — warm golden
        float sx = L+W*0.12f, sy = T+H*0.14f;
        QRadialGradient sunGlow(sx, sy, 65);
        sunGlow.setColorAt(0,    QColor(255,250,180, 80));
        sunGlow.setColorAt(0.4f, QColor(255,220,100, 40));
        sunGlow.setColorAt(1,    Qt::transparent);
        p.fillRect(QRectF(sx-65,sy-65,130,130), sunGlow);
        QRadialGradient sunBody(sx-3, sy-3, 26);
        sunBody.setColorAt(0,    QColor(255,252,200,255));
        sunBody.setColorAt(0.5f, QColor(255,235,100,240));
        sunBody.setColorAt(1,    QColor(255,210, 50,220));
        p.setPen(Qt::NoPen); p.setBrush(sunBody);
        p.drawEllipse(QPointF(sx, sy), 24, 24);
        // Sun rays
        p.setPen(QPen(QColor(255,230,80,70), 1.5f));
        for (int i = 0; i < 16; ++i) {
            float a = i * 22.5f * M_PI / 180.0f;
            float r1 = 28, r2 = 40 + (i%2)*8;
            p.drawLine(QPointF(sx+qCos(a)*r1, sy+qSin(a)*r1),
                       QPointF(sx+qCos(a)*r2, sy+qSin(a)*r2));
        }

        // Fluffy clouds
        auto drawCloud = [&](float cx, float cy, float scale, int alpha) {
            float bx = L+cx*W, by = T+cy*H;
            // Shadow
            p.setBrush(QColor(140,170,210,alpha/3));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(bx+3*scale, by+3*scale), 28*scale, 16*scale);
            p.drawEllipse(QPointF(bx+24*scale+3, by-4*scale+3), 22*scale, 15*scale);
            p.drawEllipse(QPointF(bx-20*scale+3, by-2*scale+3), 18*scale, 13*scale);
            // Cloud body
            p.setBrush(QColor(255,255,255,alpha));
            p.drawEllipse(QPointF(bx, by), 28*scale, 16*scale);
            p.drawEllipse(QPointF(bx+24*scale, by-5*scale), 22*scale, 15*scale);
            p.drawEllipse(QPointF(bx-20*scale, by-3*scale), 18*scale, 13*scale);
            p.drawEllipse(QPointF(bx+10*scale, by-10*scale), 16*scale, 14*scale);
        };
        drawCloud(0.45f, 0.09f, 1.0f, 210);
        drawCloud(0.72f, 0.07f, 0.75f, 190);
        drawCloud(0.25f, 0.12f, 0.6f, 170);

        // Rolling green hills (layered)
        auto drawHill = [&](float cy, float amp, QColor col) {
            QPainterPath hill;
            hill.moveTo(L, T+cy*H + amp*H);
            for (int xi = 0; xi <= 100; xi += 2) {
                float hx = L + xi*W/100.0f;
                float hy = T + cy*H + qSin(xi*0.063f)*amp*H + qSin(xi*0.031f)*amp*H*0.5f;
                if (xi == 0) hill.moveTo(hx, hy);
                else hill.lineTo(hx, hy);
            }
            hill.lineTo(L+W, T+H); hill.lineTo(L, T+H); hill.closeSubpath();
            p.fillPath(hill, col);
        };
        drawHill(0.60f, 0.04f, QColor(55, 140, 50, 200));  // distant hill
        drawHill(0.66f, 0.03f, QColor(65, 155, 55, 220));  // mid hill
        drawHill(0.72f, 0.025f,QColor(80, 170, 60, 230));  // near hill

        // Cherry blossom trees
        auto drawBlossomTree = [&](float tx, float bh) {
            float bx = L+tx*W;
            float by = T+H*0.70f;
            // Trunk
            p.setBrush(QColor(80,50,30,200));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(QRectF(bx-4, by, 8, H*bh), 3, 3);
            // Branch
            p.setPen(QPen(QColor(80,50,30,160), 2.5f));
            p.drawLine(QPointF(bx, by+H*bh*0.3f), QPointF(bx-20, by+H*bh*0.1f));
            p.drawLine(QPointF(bx, by+H*bh*0.3f), QPointF(bx+18, by+H*bh*0.1f));
            p.setPen(Qt::NoPen);
            // Blossom canopy
            struct Blossom { float ox,oy,s; };
            Blossom blossoms[] = {
                {0,0,28},{-22,-15,22},{22,-12,20},{-10,-28,24},{12,-26,22},
                {-30,-5,18},{28,-8,18},{0,-32,20},{-18,-38,16},{18,-35,16}
            };
            for (auto& bl : blossoms) {
                QRadialGradient bg2(bx+bl.ox-2, by+bl.oy-2, bl.s);
                bg2.setColorAt(0,    QColor(255,200,220,180));
                bg2.setColorAt(0.6f, QColor(255,170,195,160));
                bg2.setColorAt(1,    QColor(255,140,170,100));
                p.fillRect(QRectF(bx+bl.ox-bl.s, by+bl.oy-bl.s, bl.s*2, bl.s*2), bg2);
                p.setBrush(QColor(255,190,210,150));
                p.drawEllipse(QPointF(bx+bl.ox, by+bl.oy), bl.s*0.85f, bl.s*0.85f);
            }
        };
        drawBlossomTree(0.08f, 0.16f);
        drawBlossomTree(0.88f, 0.14f);

        // Falling blossom petals
        struct Petal { float x,y,rx,ry,rot,a; };
        Petal petals[] = {
            {0.12f,0.52f,4,2.5f,25,160},{0.20f,0.60f,3,2,45,140},
            {0.35f,0.48f,4.5f,2.5f,70,150},{0.55f,0.55f,3.5f,2,15,130},
            {0.65f,0.50f,4,2.5f,55,160},{0.78f,0.57f,3,2,80,140},
            {0.15f,0.68f,3.5f,2,35,120},{0.45f,0.62f,4,2.5f,60,130},
            {0.82f,0.65f,3,2,20,110},{0.92f,0.53f,4.5f,2.5f,45,150},
        };
        for (auto& pt : petals) {
            p.save();
            p.translate(L+pt.x*W, T+pt.y*H);
            p.rotate(pt.rot);
            p.setBrush(QColor(255,185,205,pt.a));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(0,0), pt.rx, pt.ry);
            p.restore();
        }

        // Wildflowers in foreground
        auto drawFlower = [&](float fx, float fy, QColor col, float scale) {
            float x = L+fx*W, y = T+fy*H;
            // stem
            p.setPen(QPen(QColor(40,120,30,160), 1.2f));
            p.drawLine(QPointF(x, y), QPointF(x+(fx>0.5f?2:-2), y+12*scale));
            p.setPen(Qt::NoPen);
            // petals
            for (int i = 0; i < 6; ++i) {
                float a = i * 60.0f * M_PI / 180.0f;
                p.setBrush(QColor(col.red(),col.green(),col.blue(),180));
                p.drawEllipse(QPointF(x+qCos(a)*5*scale, y+qSin(a)*5*scale), 3*scale, 2.5f*scale);
            }
            p.setBrush(QColor(255,240,60,220));
            p.drawEllipse(QPointF(x,y), 2.5f*scale, 2.5f*scale);
        };
        drawFlower(0.05f,0.82f,QColor(255,80,130),1.1f);
        drawFlower(0.12f,0.87f,QColor(180,80,255),1.0f);
        drawFlower(0.19f,0.83f,QColor(255,200,50),1.2f);
        drawFlower(0.30f,0.80f,QColor(255,80,130),0.9f);
        drawFlower(0.40f,0.85f,QColor(100,180,255),1.1f);
        drawFlower(0.52f,0.81f,QColor(255,130,50),1.0f);
        drawFlower(0.62f,0.84f,QColor(255,80,130),1.2f);
        drawFlower(0.72f,0.80f,QColor(180,80,255),1.0f);
        drawFlower(0.80f,0.86f,QColor(255,200,50),0.9f);
        drawFlower(0.90f,0.82f,QColor(100,200,100),1.1f);
        drawFlower(0.96f,0.79f,QColor(255,80,130),1.0f);

        // Butterfly (small decorative)
        auto drawButterfly = [&](float bx, float by, QColor col) {
            float x = L+bx*W, y = T+by*H;
            p.save(); p.translate(x,y);
            p.setBrush(QColor(col.red(),col.green(),col.blue(),160));
            p.setPen(QPen(QColor(col.red()/2,col.green()/2,col.blue()/2,100),0.5f));
            // wings
            p.drawEllipse(QPointF(-5,-3), 7.0f, 4.5f);
            p.drawEllipse(QPointF(5,-3),  7.0f, 4.5f);
            p.drawEllipse(QPointF(-4, 3), 5.5f, 3.5f);
            p.drawEllipse(QPointF(4,  3), 5.5f, 3.5f);
            // body
            p.setBrush(QColor(60,40,20,180)); p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(0,0), 1.5f, 4.5f);
            p.restore();
        };
        drawButterfly(0.48f, 0.65f, QColor(255,150,50));
        drawButterfly(0.73f, 0.60f, QColor(100,180,255));

    // ════════════════════════════════════════════════════════
    // ☀️  LJETO  (Jun, Jul, Aug)
    // ════════════════════════════════════════════════════════
    } else if (month >= 6 && month <= 8) {

        // Sky: brilliant deep blue
        QLinearGradient sky(0, 0, 0, H);
        sky.setColorAt(0.00f, QColor( 15,  80, 190));
        sky.setColorAt(0.25f, QColor( 40, 120, 215));
        sky.setColorAt(0.45f, QColor( 80, 160, 235));
        sky.setColorAt(0.50f, QColor( 15,  90, 160));  // sea surface
        sky.setColorAt(0.70f, QColor( 10,  65, 130));
        sky.setColorAt(1.00f, QColor(  5,  45, 100));
        p.fillRect(r, sky);

        // Blazing sun + halo
        float sx = L+W*0.80f, sy = T+H*0.11f;
        QRadialGradient sunHalo(sx, sy, 100);
        sunHalo.setColorAt(0,    QColor(255,250,200,100));
        sunHalo.setColorAt(0.3f, QColor(255,220,100, 60));
        sunHalo.setColorAt(0.6f, QColor(255,180, 50, 25));
        sunHalo.setColorAt(1,    Qt::transparent);
        p.fillRect(QRectF(sx-100,sy-100,200,200), sunHalo);
        QRadialGradient sunBody(sx-4, sy-4, 32);
        sunBody.setColorAt(0,    QColor(255,255,220,255));
        sunBody.setColorAt(0.4f, QColor(255,245,150,250));
        sunBody.setColorAt(0.8f, QColor(255,215, 50,240));
        sunBody.setColorAt(1,    QColor(255,180,  0,220));
        p.setPen(Qt::NoPen); p.setBrush(sunBody);
        p.drawEllipse(QPointF(sx, sy), 30, 30);
        // Long rays
        p.setPen(QPen(QColor(255,230,80,50), 1.8f));
        for (int i = 0; i < 16; ++i) {
            float a = i * 22.5f * M_PI / 180.0f;
            float r1 = 34, r2 = 52 + (i%2)*12;
            p.drawLine(QPointF(sx+qCos(a)*r1, sy+qSin(a)*r1),
                       QPointF(sx+qCos(a)*r2, sy+qSin(a)*r2));
        }
        // Shimmer rays (long diagonal)
        p.setPen(QPen(QColor(255,240,150,18), 2.5f));
        for (int i = 0; i < 5; ++i) {
            float a = (-30 + i*15) * M_PI / 180.0f;
            p.drawLine(QPointF(sx+qCos(a)*35, sy+qSin(a)*35),
                       QPointF(sx+qCos(a)*180, sy+qSin(a)*180));
        }

        // Clouds (bright summer, few)
        auto drawSummerCloud = [&](float cx, float cy, float sc, int al) {
            float bx = L+cx*W, by = T+cy*H;
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(200,220,255,al/5));
            p.drawEllipse(QPointF(bx+2*sc, by+2*sc), 22*sc, 13*sc);
            p.setBrush(QColor(255,255,255,al));
            p.drawEllipse(QPointF(bx, by), 22*sc, 13*sc);
            p.drawEllipse(QPointF(bx+18*sc, by-4*sc), 17*sc, 12*sc);
            p.drawEllipse(QPointF(bx-16*sc, by-2*sc), 15*sc, 11*sc);
            p.drawEllipse(QPointF(bx+5*sc, by-9*sc), 14*sc, 11*sc);
        };
        drawSummerCloud(0.18f, 0.08f, 1.0f, 200);
        drawSummerCloud(0.50f, 0.06f, 0.65f, 170);

        // Distant islands/coast silhouette
        QPainterPath coast;
        coast.moveTo(L, T+H*0.55f);
        coast.cubicTo(L+W*0.1f, T+H*0.50f, L+W*0.2f, T+H*0.52f, L+W*0.25f, T+H*0.55f);
        coast.lineTo(L+W*0.25f, T+H*0.62f); coast.lineTo(L, T+H*0.62f);
        coast.closeSubpath();
        p.fillPath(coast, QColor(20,80,40,120));

        // Sea surface with sparkle
        QLinearGradient seaSurf(0, T+H*0.50f, 0, T+H*0.60f);
        seaSurf.setColorAt(0, QColor(80,160,220,180));
        seaSurf.setColorAt(1, QColor(20,100,170,200));
        p.fillRect(QRectF(L, T+H*0.50f, W, H*0.10f), seaSurf);

        // Sea sparkle (sun reflections)
        p.setPen(QPen(QColor(255,255,220,80), 1.2f));
        float reflY = T+H*0.52f;
        for (int i = 0; i < 20; ++i) {
            float rx = L + (i*57+30)%100 * W/100.0f;
            float ry = reflY + (i*37)%int(H*0.07f);
            float rl = 4 + (i%3)*3;
            p.drawLine(QPointF(rx-rl,ry), QPointF(rx+rl,ry));
            if (i%3==0) p.drawLine(QPointF(rx,ry-2), QPointF(rx,ry+2));
        }

        // Waves — multiple layers
        auto drawWave = [&](float wy, float amp, int alpha, float phase) {
            p.setPen(QPen(QColor(180,220,255,alpha), 1.2f));
            QPainterPath wave;
            bool first = true;
            for (float wx = 0; wx <= W+20; wx += 3) {
                float wwy = T+wy*H + qSin((wx*0.04f)+phase)*amp + qSin((wx*0.025f)+phase*0.7f)*amp*0.5f;
                if (first) { wave.moveTo(L+wx, wwy); first=false; }
                else wave.lineTo(L+wx, wwy);
            }
            p.drawPath(wave);
        };
        drawWave(0.58f, 3.5f, 80, 0.0f);
        drawWave(0.62f, 3.0f, 65, 1.2f);
        drawWave(0.67f, 2.5f, 50, 0.6f);
        drawWave(0.72f, 2.0f, 40, 1.8f);
        drawWave(0.77f, 1.8f, 35, 0.3f);

        // Sandy beach
        QLinearGradient beach(0, T+H*0.83f, 0, T+H);
        beach.setColorAt(0,    QColor(245,218,150,230));
        beach.setColorAt(0.3f, QColor(235,205,135,240));
        beach.setColorAt(1.0f, QColor(220,190,118,250));
        p.fillRect(QRectF(L, T+H*0.83f, W, H*0.17f), beach);
        // Beach texture (subtle sand ripples)
        p.setPen(QPen(QColor(200,170,100,40), 0.8f));
        for (int i = 0; i < 8; ++i) {
            float bry = T+H*0.85f + i*H*0.018f;
            p.drawLine(QPointF(L+W*0.02f,bry), QPointF(L+W*0.98f, bry+2));
        }
        // Seashells
        auto drawShell = [&](float shx, float shy) {
            float x = L+shx*W, y = T+shy*H;
            p.setPen(QPen(QColor(200,160,120,150),0.7f));
            p.setBrush(QColor(235,210,180,180));
            p.drawEllipse(QPointF(x,y), 4.0f, 2.5f);
            p.drawLine(QPointF(x-4,y), QPointF(x+4,y));
        };
        drawShell(0.12f, 0.88f); drawShell(0.28f, 0.91f);
        drawShell(0.55f, 0.86f); drawShell(0.71f, 0.90f);
        drawShell(0.85f, 0.87f);

        // Palm tree (stylized)
        auto drawPalm = [&](float px, float ph) {
            float x = L+px*W, y = T+H;
            // trunk (curved)
            QPainterPath trunk;
            trunk.moveTo(x, y);
            trunk.cubicTo(x+8, y-H*ph*0.5f, x-5, y-H*ph*0.8f, x, y-H*ph);
            p.setPen(QPen(QColor(100,70,30,200), 5));
            p.drawPath(trunk);
            // fronds
            float tx = x, ty = y-H*ph;
            p.setPen(QPen(QColor(40,120,30,180), 1.5f));
            struct Frond { float ax,ay,ex,ey; };
            Frond fronds[] = {
                {-40,-20,0,0},{-30,-35,0,0},{-10,-45,0,0},{15,-40,0,0},
                {35,-25,0,0},{40,-10,0,0},{20,-5,0,0}
            };
            for (auto& f : fronds) {
                p.drawLine(QPointF(tx,ty), QPointF(tx+f.ax, ty+f.ay));
                // leaf detail
                for (int i = 1; i <= 4; ++i) {
                    float fx2 = tx + f.ax*i/5.0f, fy2 = ty + f.ay*i/5.0f;
                    float side = (f.ax > 0) ? 1 : -1;
                    p.drawLine(QPointF(fx2,fy2), QPointF(fx2+side*8, fy2+5));
                }
            }
        };
        drawPalm(0.04f, 0.30f);
        drawPalm(0.94f, 0.26f);

        // Seagulls (simple M shapes)
        p.setPen(QPen(QColor(220,230,255,130), 1.2f));
        struct Gull { float x,y,s; };
        Gull gulls[] = {{0.30f,0.12f,5},{0.36f,0.10f,4},{0.33f,0.07f,6},
                        {0.55f,0.15f,4},{0.60f,0.13f,5}};
        for (auto& g : gulls) {
            float gx = L+g.x*W, gy = T+g.y*H;
            p.drawArc(QRectF(gx-g.s,gy-g.s*0.5f,g.s,g.s), 0, 180*16);
            p.drawArc(QRectF(gx,    gy-g.s*0.5f,g.s,g.s), 0, 180*16);
        }

    // ════════════════════════════════════════════════════════
    // 🍂  JESEN  (Sep, Oct, Nov)
    // ════════════════════════════════════════════════════════
    } else {

        // Sky: moody amber-grey dusk
        QLinearGradient sky(0, 0, 0, H);
        sky.setColorAt(0.00f, QColor( 28,  22,  18));
        sky.setColorAt(0.20f, QColor( 55,  40,  25));
        sky.setColorAt(0.40f, QColor( 95,  65,  35));
        sky.setColorAt(0.55f, QColor(140,  90,  40));
        sky.setColorAt(0.65f, QColor( 75,  50,  20));
        sky.setColorAt(1.00f, QColor( 40,  28,  12));
        p.fillRect(r, sky);

        // Setting sun (low, orange glow on horizon)
        float sunX = L+W*0.72f, sunY = T+H*0.58f;
        QRadialGradient sunGlow(sunX, sunY, W*0.55f);
        sunGlow.setColorAt(0,    QColor(255,160, 40,120));
        sunGlow.setColorAt(0.2f, QColor(255,120, 20, 80));
        sunGlow.setColorAt(0.5f, QColor(200, 80, 10, 40));
        sunGlow.setColorAt(1,    Qt::transparent);
        p.fillRect(r, sunGlow);
        // Sun disc
        QRadialGradient sunDisc(sunX, sunY, 22);
        sunDisc.setColorAt(0,    QColor(255,200,100,200));
        sunDisc.setColorAt(0.6f, QColor(255,140, 40,180));
        sunDisc.setColorAt(1,    QColor(230, 80, 10,150));
        p.setPen(Qt::NoPen); p.setBrush(sunDisc);
        p.drawEllipse(QPointF(sunX, sunY), 18, 18);

        // Fog/mist layers
        auto drawMist = [&](float my, float mh, int alpha) {
            QLinearGradient mist(0, T+my*H, 0, T+(my+mh)*H);
            mist.setColorAt(0, Qt::transparent);
            mist.setColorAt(0.3f, QColor(180,150,100,alpha));
            mist.setColorAt(0.7f, QColor(160,130, 80,alpha));
            mist.setColorAt(1, Qt::transparent);
            p.fillRect(QRectF(L, T+my*H, W, mh*H), mist);
        };
        drawMist(0.45f, 0.12f, 35);
        drawMist(0.55f, 0.10f, 45);

        // Layered forest silhouettes (background to foreground)
        struct TreeSil { float x; float h; QColor col; };
        // Background trees (small, dark)
        p.setPen(Qt::NoPen);
        auto drawForestLayer = [&](float baseY, float treeH, float spacing, QColor col, int count) {
            for (int i = 0; i < count; ++i) {
                float tx = L + (i * W/(count-1));
                float th = treeH * (0.85f + (i*17%30)*0.01f);
                QPainterPath tree;
                tree.moveTo(tx, T+baseY*H);
                // Irregular tree top
                tree.cubicTo(tx-th*0.35f, T+baseY*H-th*H*0.5f,
                             tx-th*0.25f, T+baseY*H-th*H,
                             tx, T+baseY*H-th*H);
                tree.cubicTo(tx+th*0.25f, T+baseY*H-th*H,
                             tx+th*0.35f, T+baseY*H-th*H*0.5f,
                             tx, T+baseY*H);
                p.fillPath(tree, col);
            }
        };
        drawForestLayer(0.62f, 0.22f, 0.06f, QColor(35,20,8,180), 18);
        drawForestLayer(0.68f, 0.20f, 0.07f, QColor(80,45,15,200), 16);
        drawForestLayer(0.74f, 0.18f, 0.065f,QColor(120,65,20,220), 15);

        // Detailed autumn trees (midground, colourful)
        auto drawAutumnTree = [&](float tx, float baseY, float trunkH, QColor canopy) {
            float x = L+tx*W, y = T+baseY*H;
            // Trunk
            p.setBrush(QColor(55,35,15,220));
            p.setPen(Qt::NoPen);
            QPainterPath trunk;
            trunk.moveTo(x-5, y);
            trunk.cubicTo(x-4, y-trunkH*H*0.5f, x+3, y-trunkH*H*0.7f, x, y-trunkH*H);
            trunk.cubicTo(x+4, y-trunkH*H, x+6, y-trunkH*H*0.5f, x+5, y);
            p.fillPath(trunk, QColor(55,35,15,220));
            // Branches
            p.setPen(QPen(QColor(60,38,15,180), 2.0f));
            p.drawLine(QPointF(x, y-trunkH*H*0.6f), QPointF(x-25, y-trunkH*H*0.9f));
            p.drawLine(QPointF(x, y-trunkH*H*0.7f), QPointF(x+22, y-trunkH*H*0.95f));
            p.setPen(Qt::NoPen);
            // Layered canopy
            QColor cols[3] = {
                canopy,
                QColor(qMin(canopy.red()+20,255), qMax(canopy.green()-15,0), qMax(canopy.blue()-5,0),canopy.alpha()),
                QColor(qMax(canopy.red()-20,0), canopy.green(), qMax(canopy.blue()-10,0),canopy.alpha()-20)
            };
            float offsets[3][3] = {{0,-40,32},{-22,-52,25},{20,-48,27}};
            for (int l = 0; l < 3; ++l) {
                QRadialGradient cg(x+offsets[l][0], y-trunkH*H+offsets[l][1], offsets[l][2]+8);
                cg.setColorAt(0,    QColor(cols[l].red(),cols[l].green(),cols[l].blue(),cols[l].alpha()));
                cg.setColorAt(1,    QColor(cols[l].red(),cols[l].green(),cols[l].blue(),80));
                p.fillRect(QRectF(x+offsets[l][0]-offsets[l][2]-8, y-trunkH*H+offsets[l][1]-offsets[l][2]-8,
                                  (offsets[l][2]+8)*2, (offsets[l][2]+8)*2), cg);
                p.setBrush(cols[l]);
                p.drawEllipse(QPointF(x+offsets[l][0], y-trunkH*H+offsets[l][1]), offsets[l][2], offsets[l][2]*0.85f);
            }
        };
        drawAutumnTree(0.12f, 0.78f, 0.18f, QColor(220, 80, 20, 210));
        drawAutumnTree(0.30f, 0.76f, 0.20f, QColor(200,130, 15, 210));
        drawAutumnTree(0.55f, 0.77f, 0.17f, QColor(180, 50, 10, 210));
        drawAutumnTree(0.75f, 0.79f, 0.19f, QColor(215,110, 20, 210));
        drawAutumnTree(0.92f, 0.76f, 0.16f, QColor(195, 70, 15, 200));

        // Ground — dark earthy
        QLinearGradient ground(0, T+H*0.78f, 0, T+H);
        ground.setColorAt(0,    QColor(55, 35, 12, 230));
        ground.setColorAt(0.3f, QColor(42, 26,  8, 240));
        ground.setColorAt(1.0f, QColor(28, 18,  5, 255));
        p.fillRect(QRectF(L, T+H*0.78f, W, H*0.22f), ground);

        // Fallen leaves on ground
        struct Leaf { float x,y,rx,ry,rot; QColor col; };
        Leaf leaves[] = {
            {0.05f,0.84f,6,4,25,QColor(220,75,15,200)},
            {0.11f,0.87f,5,3,70,QColor(200,120,10,190)},
            {0.18f,0.83f,7,4,15,QColor(180,45,8,200)},
            {0.25f,0.86f,5,3,55,QColor(215,95,18,195)},
            {0.33f,0.82f,6,4,40,QColor(200,130,15,185)},
            {0.42f,0.85f,5,3.5f,80,QColor(225,80,20,200)},
            {0.50f,0.81f,7,4,20,QColor(185,50,10,195)},
            {0.58f,0.84f,5,3,65,QColor(210,105,15,190)},
            {0.66f,0.83f,6,4,35,QColor(220,75,18,200)},
            {0.74f,0.86f,5,3,75,QColor(195,60,12,195)},
            {0.82f,0.82f,7,4,10,QColor(215,115,20,185)},
            {0.90f,0.85f,5,3,50,QColor(200,80,15,200)},
            {0.96f,0.83f,6,4,30,QColor(190,45,8,195)},
        };
        for (auto& lf : leaves) {
            p.save();
            p.translate(L+lf.x*W, T+lf.y*H);
            p.rotate(lf.rot);
            p.setBrush(lf.col);
            p.setPen(Qt::NoPen);
            QPainterPath leafShape;
            leafShape.moveTo(0, -lf.ry);
            leafShape.cubicTo(lf.rx*0.7f,-lf.ry*0.5f, lf.rx,lf.ry*0.3f, 0,lf.ry);
            leafShape.cubicTo(-lf.rx,lf.ry*0.3f, -lf.rx*0.7f,-lf.ry*0.5f, 0,-lf.ry);
            p.fillPath(leafShape, lf.col);
            // Leaf vein
            p.setPen(QPen(QColor(lf.col.red()/2,lf.col.green()/2,lf.col.blue()/2,120), 0.6f));
            p.drawLine(QPointF(0,-lf.ry), QPointF(0,lf.ry));
            p.restore();
        }

        // Falling leaves (mid-air)
        struct FallingLeaf { float x,y,rx,ry,rot; QColor col; };
        FallingLeaf falling[] = {
            {0.08f,0.45f,5,3,40,QColor(220,80,15,160)},
            {0.22f,0.52f,4,2.5f,65,QColor(200,110,12,150)},
            {0.38f,0.40f,5.5f,3.5f,20,QColor(185,50,8,155)},
            {0.53f,0.48f,4,2.5f,80,QColor(215,95,18,145)},
            {0.67f,0.43f,5,3,35,QColor(210,75,15,160)},
            {0.78f,0.50f,4.5f,3,55,QColor(195,125,10,150)},
            {0.88f,0.44f,5,3,15,QColor(225,65,20,155)},
        };
        for (auto& fl : falling) {
            p.save();
            p.translate(L+fl.x*W, T+fl.y*H);
            p.rotate(fl.rot);
            QPainterPath lsh;
            lsh.moveTo(0,-fl.ry);
            lsh.cubicTo(fl.rx*0.7f,-fl.ry*0.5f, fl.rx,fl.ry*0.3f, 0,fl.ry);
            lsh.cubicTo(-fl.rx,fl.ry*0.3f,-fl.rx*0.7f,-fl.ry*0.5f, 0,-fl.ry);
            p.fillPath(lsh, fl.col);
            p.restore();
        }

        // Spiderweb (autumnal detail)
        auto drawWeb = [&](float wx, float wy, float wr) {
            float x = L+wx*W, y = T+wy*H;
            p.setPen(QPen(QColor(200,200,220,50), 0.6f));
            for (int i = 0; i < 8; ++i) {
                float a = i*45.0f*M_PI/180.0f;
                p.drawLine(QPointF(x,y), QPointF(x+qCos(a)*wr, y+qSin(a)*wr));
            }
            for (int ring = 1; ring <= 3; ++ring) {
                float rr = wr * ring / 3.5f;
                QPainterPath webRing;
                for (int i = 0; i <= 8; ++i) {
                    float a = i*45.0f*M_PI/180.0f;
                    QPointF pt(x+qCos(a)*rr, y+qSin(a)*rr);
                    if (i==0) webRing.moveTo(pt);
                    else webRing.lineTo(pt);
                }
                p.drawPath(webRing);
            }
        };
        drawWeb(0.03f, 0.56f, 16);
        drawWeb(0.97f, 0.52f, 14);
    }

    // ── Vignette (darkens edges to keep calendar cells readable) ──
    QRadialGradient vignette(L+W*0.5f, T+H*0.5f, W*0.75f);
    vignette.setColorAt(0,    Qt::transparent);
    vignette.setColorAt(0.65f,Qt::transparent);
    vignette.setColorAt(1,    QColor(0,0,0,80));
    p.fillRect(r, vignette);

    // ── Semi-transparent overlay on calendar grid area ──
    // This ensures numbers/text remain crisp regardless of background
    QLinearGradient cellOverlay(0, T+H*0.20f, 0, T+H);
    cellOverlay.setColorAt(0,    QColor(5,5,15,90));
    cellOverlay.setColorAt(0.15f,QColor(5,5,15,75));
    cellOverlay.setColorAt(1.0f, QColor(5,5,15,70));
    p.fillRect(QRectF(L, T+H*0.20f, W, H*0.80f), cellOverlay);

    p.restore();
}

// ── paintEvent ───────────────────────────────────────────────────
void CalendarWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    int month = m_currentDate.month();

    // Draw seasonal background (photo or painted)
    int season = currentSeason();
    if (!m_seasonPixmaps[season].isNull()) {
        // Draw photo — scale to fill, keep aspect ratio
        QRect target = rect();
        QPixmap scaled = m_seasonPixmaps[season].scaled(
            target.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        // Center crop
        int ox = (scaled.width()  - target.width())  / 2;
        int oy = (scaled.height() - target.height()) / 2;
        p.drawPixmap(target, scaled, QRect(ox, oy, target.width(), target.height()));
        // Dark overlay so calendar text is readable
        QLinearGradient overlay(0, 0, 0, height());
        overlay.setColorAt(0.00f, QColor(0,0,0,130));
        overlay.setColorAt(0.18f, QColor(0,0,0,110));
        overlay.setColorAt(0.50f, QColor(0,0,0,105));
        overlay.setColorAt(1.00f, QColor(0,0,0,115));
        p.fillRect(target, overlay);
        // Subtle colour tint per season
        const QColor tints[4] = {
            QColor( 40, 80,160, 30),  // Zima — plava
            QColor( 60,160, 60, 25),  // Proljece — zelena
            QColor(180,140,  0, 25),  // Ljeto — zlatna
            QColor(160, 70, 10, 30),  // Jesen — narandzasta
        };
        p.fillRect(target, tints[season]);
    } else {
        drawSeasonalBackground(p, rect(), month);
    }

    // Camera hint button (top-right corner — right-click for context menu)
    {
        const QString icons[4] = {"❄️","🌸","☀️","🍂"};
        bool hasImg = !m_seasonPixmaps[season].isNull();
        QString hint = hasImg
            ? (icons[season] + " Desni klik → promijeni sliku")
            : (icons[season] + " Desni klik → dodaj sliku");
        p.save();
        // Pill background
        QFont hintFont; hintFont.setPixelSize(11); p.setFont(hintFont);
        QFontMetrics hfm(hintFont);
        QRect htr = hfm.boundingRect(hint);
        int px = width() - htr.width() - 22, py = 6;
        int pw = htr.width() + 16, ph = 20;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0,0,0,110));
        p.drawRoundedRect(px, py, pw, ph, 10, 10);
        // Border glow
        p.setPen(QPen(hasImg ? QColor(100,200,100,120) : QColor(153,102,204,120), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(px, py, pw, ph, 10, 10);
        // Text
        p.setPen(QColor(220,220,255,200));
        p.drawText(QRect(px+8, py+1, pw-10, ph), Qt::AlignVCenter | Qt::AlignLeft, hint);
        p.restore();
    }

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
