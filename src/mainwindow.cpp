#include "mainwindow.h"
#include "calendarwidget.h"
#include "eventmanager.h"
#include "musicplayer.h"
#include "orthodoxholidays.h"
#include "style.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QSplitter>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimeEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QApplication>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QDateTime>
#include <QScrollArea>
#include <QColorDialog>
#include <QSizePolicy>

// ─────────────────────────────────────────────────────────────────
// NAV BUTTON helper
static QPushButton* makeNavBtn(const QString& txt, const QString& tip,
                                 QWidget *parent)
{
    auto *b = new QPushButton(txt, parent);
    b->setToolTip(tip);
    b->setFixedSize(38, 38);
    b->setStyleSheet(R"(
        QPushButton {
            background: rgba(255,255,255,0.06);
            border: 1px solid rgba(255,255,255,0.08);
            border-radius: 10px;
            color: #ccccee;
            font-size: 16px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(83,52,131,0.5);
            border-color: rgba(140,100,200,0.5);
            color: #ffffff;
        }
        QPushButton:pressed { background: rgba(83,52,131,0.8); }
    )");
    return b;
}

static QPushButton* makeSideBtn(const QString& txt, const QString& color,
                                  QWidget *parent)
{
    auto *b = new QPushButton(txt, parent);
    b->setStyleSheet(QString(R"(
        QPushButton {
            background: rgba(%1, 0.18);
            border: 1px solid rgba(%1, 0.45);
            border-radius: 8px;
            color: rgb(%1);
            font-size: 12px;
            font-weight: 600;
            padding: 5px 12px;
        }
        QPushButton:hover { background: rgba(%1, 0.35); }
        QPushButton:pressed { background: rgba(%1, 0.6); }
        QPushButton:disabled { opacity: 0.4; }
    )").arg(color));
    return b;
}

// ─────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_events = new EventManager(this);
    m_displayDate = QDate::currentDate();
    m_selectedDate = m_displayDate;

    connect(m_events, &EventManager::reminderTriggered,
            this, &MainWindow::onReminderTriggered);

    setWindowTitle("☦  Pravoslavni Kalendar");
    setMinimumSize(900, 620);
    resize(1280, 800);
    setStyleSheet(appStyleSheet());

    setupUI();
    setupTrayIcon();

    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    m_clockTimer->start(1000);
    updateClock();
    applyMonthLabel();
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────────────────────────
void MainWindow::setupUI() {
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *outerV = new QVBoxLayout(central);
    outerV->setContentsMargins(0, 0, 0, 0);
    outerV->setSpacing(0);

    // ── Navigation Bar ────────────────────────────────────────────
    m_navBar = new QFrame(central);
    m_navBar->setFixedHeight(60);
    m_navBar->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #0d0d28, stop:0.5 #130d2e, stop:1 #0d0d28);
            border-bottom: 1px solid rgba(255,255,255,0.08);
        }
    )");
    auto *navLayout = new QHBoxLayout(m_navBar);
    navLayout->setContentsMargins(16, 0, 16, 0);
    navLayout->setSpacing(8);

    // App icon + title
    auto *appTitle = new QLabel("☦  Pravoslavni Kalendar", m_navBar);
    appTitle->setStyleSheet("color:#9966cc; font-size:15px; font-weight:700; letter-spacing:1px;");

    m_lblClock = new QLabel("", m_navBar);
    m_lblClock->setStyleSheet("color:#666688; font-size:12px; min-width:140px;");
    m_lblClock->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_btnPrev    = makeNavBtn("◀", "Prethodni mjesec", m_navBar);
    m_btnNext    = makeNavBtn("▶", "Sljedeći mjesec",  m_navBar);
    m_btnToday   = makeNavBtn("⊙", "Danas",             m_navBar);
    m_btnToday->setToolTip("Idi na danas");

    m_lblMonth = new QLabel("", m_navBar);
    m_lblMonth->setAlignment(Qt::AlignCenter);
    m_lblMonth->setStyleSheet(
        "color:#e0d0ff; font-size:20px; font-weight:700; "
        "min-width:260px; letter-spacing:1px;");

    m_btnFullscreen = makeNavBtn("⛶", "Puni ekran (F11)", m_navBar);
    m_btnAbout      = makeNavBtn("ℹ", "O programu",       m_navBar);

    navLayout->addWidget(appTitle);
    navLayout->addStretch();
    navLayout->addWidget(m_btnPrev);
    navLayout->addWidget(m_lblMonth);
    navLayout->addWidget(m_btnNext);
    navLayout->addWidget(m_btnToday);
    navLayout->addStretch();
    navLayout->addWidget(m_lblClock);
    navLayout->addWidget(m_btnFullscreen);
    navLayout->addWidget(m_btnAbout);

    outerV->addWidget(m_navBar);

    // ── Main Splitter (calendar | side panel) ─────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal, central);
    splitter->setHandleWidth(1);
    outerV->addWidget(splitter, 1);

    // Left: Calendar + Music Player
    auto *leftWidget = new QWidget(splitter);
    auto *leftV = new QVBoxLayout(leftWidget);
    leftV->setContentsMargins(0, 0, 0, 0);
    leftV->setSpacing(0);

    m_calendar = new CalendarWidget(m_events, leftWidget);
    leftV->addWidget(m_calendar, 1);

    // Music player panel
    auto *playerFrame = new QFrame(leftWidget);
    playerFrame->setFixedHeight(160);
    playerFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #0d0d25, stop:1 #080818);
            border-top: 1px solid rgba(255,255,255,0.07);
        }
    )");
    auto *playerV = new QVBoxLayout(playerFrame);
    playerV->setContentsMargins(0, 0, 0, 0);
    m_player = new MusicPlayer(playerFrame);
    playerV->addWidget(m_player);
    leftV->addWidget(playerFrame);

    splitter->addWidget(leftWidget);

    // Right: Side Panel
    m_sidePanel = new QFrame(splitter);
    m_sidePanel->setMinimumWidth(240);
    m_sidePanel->setMaximumWidth(340);
    m_sidePanel->setStyleSheet(R"(
        QFrame {
            background: #0c0c24;
            border-left: 1px solid rgba(255,255,255,0.07);
        }
    )");
    auto *sideV = new QVBoxLayout(m_sidePanel);
    sideV->setContentsMargins(14, 16, 14, 14);
    sideV->setSpacing(12);

    // Selected date display
    m_lblSelectedDate = new QLabel("", m_sidePanel);
    m_lblSelectedDate->setAlignment(Qt::AlignCenter);
    m_lblSelectedDate->setStyleSheet(R"(
        color: #e0d0ff;
        font-size: 16px;
        font-weight: 700;
        background: rgba(83,52,131,0.15);
        border: 1px solid rgba(83,52,131,0.3);
        border-radius: 10px;
        padding: 10px;
    )");
    m_lblSelectedDate->setWordWrap(true);
    sideV->addWidget(m_lblSelectedDate);

    // Holiday info
    m_lblHolidayInfo = new QLabel("", m_sidePanel);
    m_lblHolidayInfo->setAlignment(Qt::AlignCenter);
    m_lblHolidayInfo->setWordWrap(true);
    m_lblHolidayInfo->setStyleSheet(R"(
        color: #e94560;
        font-size: 12px;
        background: rgba(233,69,96,0.1);
        border: 1px solid rgba(233,69,96,0.2);
        border-radius: 8px;
        padding: 8px;
    )");
    m_lblHolidayInfo->setVisible(false);
    sideV->addWidget(m_lblHolidayInfo);

    // Events section
    auto *evHeader = new QHBoxLayout();
    auto *evTitle = new QLabel("📅  Događaji", m_sidePanel);
    evTitle->setStyleSheet("color:#9988bb; font-size:12px; font-weight:600;");
    evHeader->addWidget(evTitle);
    evHeader->addStretch();
    sideV->addLayout(evHeader);

    m_eventList = new QListWidget(m_sidePanel);
    m_eventList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255,255,255,0.03);
            border: 1px solid rgba(255,255,255,0.07);
            border-radius: 10px;
            color: #e0e0f0;
            font-size: 12px;
        }
        QListWidget::item {
            padding: 7px 10px;
            border-bottom: 1px solid rgba(255,255,255,0.05);
            border-radius: 6px;
        }
        QListWidget::item:hover { background: rgba(255,255,255,0.06); }
        QListWidget::item:selected { background: rgba(83,52,131,0.35); color: #fff; }
    )");
    m_eventList->setMinimumHeight(120);
    sideV->addWidget(m_eventList, 1);

    // Event buttons
    auto *evBtns = new QHBoxLayout();
    evBtns->setSpacing(6);
    m_btnAddEvent  = makeSideBtn("+ Dodaj",  "83,200,83",  m_sidePanel);
    m_btnEditEvent = makeSideBtn("✎ Uredi",  "100,140,220", m_sidePanel);
    m_btnDelEvent  = makeSideBtn("✕ Briši", "233,69,96",  m_sidePanel);
    m_btnEditEvent->setEnabled(false);
    m_btnDelEvent->setEnabled(false);
    evBtns->addWidget(m_btnAddEvent);
    evBtns->addWidget(m_btnEditEvent);
    evBtns->addWidget(m_btnDelEvent);
    sideV->addLayout(evBtns);

    // Legend
    auto *legendBox = new QGroupBox("Legenda", m_sidePanel);
    auto *legV = new QVBoxLayout(legendBox);
    legV->setSpacing(4);
    auto addLegend = [&](const QString& color, const QString& text) {
        auto *row = new QHBoxLayout();
        auto *dot = new QLabel("●", legendBox);
        dot->setStyleSheet(QString("color:%1; font-size:16px;").arg(color));
        dot->setFixedWidth(20);
        auto *lbl = new QLabel(text, legendBox);
        lbl->setStyleSheet("color:#8888aa; font-size:11px;");
        row->addWidget(dot);
        row->addWidget(lbl);
        row->addStretch();
        legV->addLayout(row);
    };
    addLegend("#e94560", "Veliki pravoslavni praznik");
    addLegend("#c85060", "Manji praznik / nedjelja");
    addLegend("#f5a623", "Danas");
    addLegend("#533483", "Događaj");
    sideV->addWidget(legendBox);

    splitter->addWidget(m_sidePanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    // ── Connections ───────────────────────────────────────────────
    connect(m_btnPrev,       &QPushButton::clicked, this, &MainWindow::prevMonth);
    connect(m_btnNext,       &QPushButton::clicked, this, &MainWindow::nextMonth);
    connect(m_btnToday,      &QPushButton::clicked, this, &MainWindow::goToToday);
    connect(m_btnFullscreen, &QPushButton::clicked, this, &MainWindow::toggleFullScreen);
    connect(m_btnAbout,      &QPushButton::clicked, this, &MainWindow::showAboutDialog);
    connect(m_btnAddEvent,   &QPushButton::clicked,
            this, [this]{ showEventDialog(m_selectedDate); });
    connect(m_btnEditEvent,  &QPushButton::clicked, this, &MainWindow::editSelectedEvent);
    connect(m_btnDelEvent,   &QPushButton::clicked, this, &MainWindow::deleteSelectedEvent);
    connect(m_eventList, &QListWidget::itemSelectionChanged, this, [this]{
        bool sel = !m_eventList->selectedItems().isEmpty();
        m_btnEditEvent->setEnabled(sel);
        m_btnDelEvent->setEnabled(sel);
    });
    connect(m_eventList, &QListWidget::itemDoubleClicked,
            this, [this]{ editSelectedEvent(); });
    connect(m_calendar, &CalendarWidget::dateClicked,
            this, &MainWindow::onDateClicked);
    connect(m_calendar, &CalendarWidget::dateDoubleClicked,
            this, &MainWindow::onDateDoubleClicked);
    connect(m_calendar, &CalendarWidget::monthChanged,
            this, &MainWindow::onMonthChanged);

    // Initial selection
    onDateClicked(m_selectedDate);
}

// ─────────────────────────────────────────────────────────────────
void MainWindow::setupTrayIcon() {
    m_tray = new QSystemTrayIcon(this);
    // Simple cross icon using QPixmap
    QPixmap pm(32, 32);
    pm.fill(Qt::transparent);
    QPainter pp(&pm);
    pp.setRenderHint(QPainter::Antialiasing);
    pp.setPen(QPen(QColor("#9966cc"), 4, Qt::SolidLine, Qt::RoundCap));
    pp.drawLine(16, 4, 16, 28);
    pp.drawLine(8, 12, 24, 12);
    m_tray->setIcon(QIcon(pm));
    m_tray->setToolTip("Pravoslavni Kalendar");

    auto *menu = new QMenu(this);
    menu->setStyleSheet("QMenu{background:#12122e;color:#e0e0f0;border:1px solid rgba(255,255,255,0.1);}"
                        "QMenu::item:selected{background:#533483;}");
    menu->addAction("Prikaži", this, [this]{ show(); raise(); activateWindow(); });
    menu->addSeparator();
    menu->addAction("Zatvori", qApp, &QApplication::quit);
    m_tray->setContextMenu(menu);
    m_tray->show();
    connect(m_tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason r){
        if (r == QSystemTrayIcon::DoubleClick) { show(); raise(); activateWindow(); }
    });
}

// ─────────────────────────────────────────────────────────────────
void MainWindow::applyMonthLabel() {
    static const char* months[] = {
        "", "Januar", "Februar", "Mart", "April", "Maj", "Jun",
        "Jul", "Avgust", "Septembar", "Oktobar", "Novembar", "Decembar"
    };
    m_lblMonth->setText(QString("☦  %1  %2")
        .arg(months[m_displayDate.month()])
        .arg(m_displayDate.year()));
}

void MainWindow::prevMonth() {
    m_displayDate = m_displayDate.addMonths(-1);
    m_calendar->setMonth(m_displayDate.year(), m_displayDate.month());
    applyMonthLabel();
}

void MainWindow::nextMonth() {
    m_displayDate = m_displayDate.addMonths(1);
    m_calendar->setMonth(m_displayDate.year(), m_displayDate.month());
    applyMonthLabel();
}

void MainWindow::goToToday() {
    m_displayDate = QDate::currentDate();
    m_calendar->setMonth(m_displayDate.year(), m_displayDate.month());
    onDateClicked(m_displayDate);
    applyMonthLabel();
}

void MainWindow::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
        m_btnFullscreen->setText("⛶");
        m_btnFullscreen->setToolTip("Puni ekran (F11)");
    } else {
        showFullScreen();
        m_btnFullscreen->setText("⊞");
        m_btnFullscreen->setToolTip("Normalni prozor (F11)");
    }
}

void MainWindow::onMonthChanged(int year, int month) {
    m_displayDate = QDate(year, month, 1);
    applyMonthLabel();
}

void MainWindow::onDateClicked(const QDate& date) {
    m_selectedDate = date;
    static const char* days[] = {"","Ponedeljak","Utorak","Srijeda",
                                   "Četvrtak","Petak","Subota","Nedjelja"};
    static const char* months[] = {"","Januar","Februar","Mart","April","Maj","Jun",
                                    "Jul","Avgust","Septembar","Oktobar","Novembar","Decembar"};
    m_lblSelectedDate->setText(
        QString("<b>%1</b><br>%2. %3 %4.")
        .arg(days[date.dayOfWeek()])
        .arg(date.day())
        .arg(months[date.month()])
        .arg(date.year()));

    // Holiday info
    HolidayInfo hi = OrthodoxHolidays::getHolidayInfo(date, date.year());
    if (!hi.name.isEmpty()) {
        m_lblHolidayInfo->setText("☦  " + hi.name);
        m_lblHolidayInfo->setVisible(true);
    } else {
        m_lblHolidayInfo->setVisible(false);
    }
    refreshEventList(date);
}

void MainWindow::onDateDoubleClicked(const QDate& date) {
    showEventDialog(date);
}

void MainWindow::refreshEventList(const QDate& date) {
    m_eventList->clear();
    auto events = m_events->eventsForDate(date);
    if (events.isEmpty()) {
        auto *placeholder = new QListWidgetItem("  Nema događaja za ovaj dan");
        placeholder->setForeground(QColor("#444466"));
        placeholder->setFlags(Qt::NoItemFlags);
        m_eventList->addItem(placeholder);
    } else {
        for (const auto& ev : events) {
            QString timeStr = ev.time.isValid() && !ev.time.isNull()
                ? ev.time.toString("HH:mm") + "  " : "";
            QString reminder = ev.hasReminder ? " 🔔" : "";
            auto *item = new QListWidgetItem(
                QString("● %1%2%3").arg(timeStr, ev.title, reminder));
            item->setData(Qt::UserRole, ev.id);
            item->setForeground(ev.color);
            m_eventList->addItem(item);
        }
    }
    m_btnEditEvent->setEnabled(false);
    m_btnDelEvent->setEnabled(false);
}

// ─────────────────────────────────────────────────────────────────
void MainWindow::showEventDialog(const QDate& date, const QString& existingId) {
    // Load existing event if editing
    CalendarEvent existing;
    bool isEdit = !existingId.isEmpty();
    if (isEdit) {
        for (const auto& ev : m_events->eventsForDate(date)) {
            if (ev.id == existingId) { existing = ev; break; }
        }
    }

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle(isEdit ? "Uredi događaj" : "Novi događaj");
    dlg->setModal(true);
    dlg->setMinimumWidth(400);
    dlg->setStyleSheet(R"(
        QDialog {
            background: #0e0e2a;
            color: #e0e0f0;
        }
    )");

    auto *layout = new QVBoxLayout(dlg);
    layout->setSpacing(14);
    layout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto *dlgTitle = new QLabel(isEdit ? "✎  Uredi događaj" : "＋  Novi događaj", dlg);
    dlgTitle->setStyleSheet("color:#9966cc; font-size:16px; font-weight:700;");
    layout->addWidget(dlgTitle);

    // Date display
    static const char* months[] = {"","Januar","Februar","Mart","April","Maj","Jun",
                                    "Jul","Avgust","Septembar","Oktobar","Novembar","Decembar"};
    auto *dateLbl = new QLabel(QString("📅  %1. %2 %3.")
        .arg(date.day()).arg(months[date.month()]).arg(date.year()), dlg);
    dateLbl->setStyleSheet("color:#666688; font-size:12px;");
    layout->addWidget(dateLbl);

    // Event title
    auto *lblTitle = new QLabel("Naziv događaja *", dlg);
    lblTitle->setStyleSheet("color:#aaaacc; font-size:12px;");
    auto *edtTitle = new QLineEdit(dlg);
    edtTitle->setPlaceholderText("Unesi naziv događaja...");
    if (isEdit) edtTitle->setText(existing.title);
    layout->addWidget(lblTitle);
    layout->addWidget(edtTitle);

    // Time
    auto *timeRow = new QHBoxLayout();
    auto *chkTime = new QCheckBox("Postavi vrijeme", dlg);
    chkTime->setStyleSheet("color:#aaaacc;");
    auto *timePicker = new QTimeEdit(dlg);
    timePicker->setDisplayFormat("HH:mm");
    timePicker->setEnabled(false);
    if (isEdit && existing.time.isValid()) {
        chkTime->setChecked(true);
        timePicker->setTime(existing.time);
        timePicker->setEnabled(true);
    }
    connect(chkTime, &QCheckBox::toggled, timePicker, &QTimeEdit::setEnabled);
    timeRow->addWidget(chkTime);
    timeRow->addWidget(timePicker);
    timeRow->addStretch();
    layout->addLayout(timeRow);

    // Description
    auto *lblDesc = new QLabel("Opis (opciono)", dlg);
    lblDesc->setStyleSheet("color:#aaaacc; font-size:12px;");
    auto *edtDesc = new QTextEdit(dlg);
    edtDesc->setPlaceholderText("Dodaj opis...");
    edtDesc->setFixedHeight(80);
    if (isEdit) edtDesc->setPlainText(existing.description);
    layout->addWidget(lblDesc);
    layout->addWidget(edtDesc);

    // Reminder
    auto *remBox = new QGroupBox("Podsjetnik", dlg);
    auto *remV = new QVBoxLayout(remBox);
    auto *chkRem = new QCheckBox("Uključi podsjetnik", dlg);
    chkRem->setChecked(isEdit ? existing.hasReminder : false);
    auto *remRow = new QHBoxLayout();
    auto *spinRem = new QSpinBox(dlg);
    spinRem->setRange(1, 1440);
    spinRem->setValue(isEdit ? existing.reminderMinsBefore : 15);
    spinRem->setSuffix(" min ranije");
    spinRem->setEnabled(chkRem->isChecked());
    connect(chkRem, &QCheckBox::toggled, spinRem, &QSpinBox::setEnabled);
    remRow->addWidget(chkRem);
    remRow->addWidget(spinRem);
    remRow->addStretch();
    remV->addLayout(remRow);
    layout->addWidget(remBox);

    // Color picker
    QColor eventColor = isEdit ? existing.color : QColor("#533483");
    auto *colorRow = new QHBoxLayout();
    auto *colorLbl = new QLabel("Boja događaja:", dlg);
    colorLbl->setStyleSheet("color:#aaaacc; font-size:12px;");
    auto *colorBtn = new QPushButton(dlg);
    colorBtn->setFixedSize(60, 28);
    auto updateColorBtn = [&](const QColor& c) {
        colorBtn->setStyleSheet(QString(
            "QPushButton{background:%1;border:2px solid rgba(255,255,255,0.2);"
            "border-radius:6px;}"
            "QPushButton:hover{border-color:rgba(255,255,255,0.5);}").arg(c.name()));
    };
    updateColorBtn(eventColor);
    connect(colorBtn, &QPushButton::clicked, [&]{
        QColor c = QColorDialog::getColor(eventColor, dlg, "Odaberi boju");
        if (c.isValid()) { eventColor = c; updateColorBtn(c); }
    });
    colorRow->addWidget(colorLbl);
    colorRow->addWidget(colorBtn);
    colorRow->addStretch();
    layout->addLayout(colorRow);

    // Buttons
    auto *btnBox = new QHBoxLayout();
    btnBox->addStretch();
    auto *btnCancel = new QPushButton("Odustani", dlg);
    btnCancel->setStyleSheet(R"(
        QPushButton{background:rgba(255,255,255,0.06);border:1px solid rgba(255,255,255,0.1);
        border-radius:8px;color:#888;padding:8px 18px;}
        QPushButton:hover{background:rgba(255,255,255,0.1);}
    )");
    auto *btnSave = new QPushButton(isEdit ? "Sačuvaj" : "Dodaj događaj", dlg);
    btnSave->setStyleSheet(R"(
        QPushButton{background:rgba(83,52,131,0.5);border:1px solid #533483;
        border-radius:8px;color:#e0d0ff;padding:8px 22px;font-weight:600;}
        QPushButton:hover{background:rgba(83,52,131,0.8);}
        QPushButton:pressed{background:#533483;}
    )");
    btnBox->addWidget(btnCancel);
    btnBox->addWidget(btnSave);
    layout->addLayout(btnBox);

    connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, dlg, [&]{
        if (edtTitle->text().trimmed().isEmpty()) {
            edtTitle->setStyleSheet(edtTitle->styleSheet() + "border-color:#e94560;");
            return;
        }
        CalendarEvent ev;
        ev.id                 = isEdit ? existing.id : "";
        ev.date               = date;
        ev.time               = chkTime->isChecked() ? timePicker->time() : QTime();
        ev.title              = edtTitle->text().trimmed();
        ev.description        = edtDesc->toPlainText().trimmed();
        ev.hasReminder        = chkRem->isChecked();
        ev.reminderMinsBefore = spinRem->value();
        ev.color              = eventColor;
        if (isEdit) m_events->updateEvent(ev);
        else        m_events->addEvent(ev);
        refreshEventList(date);
        dlg->accept();
    });

    dlg->exec();
    dlg->deleteLater();
}

void MainWindow::editSelectedEvent() {
    auto items = m_eventList->selectedItems();
    if (items.isEmpty()) return;
    QString id = items.first()->data(Qt::UserRole).toString();
    if (!id.isEmpty()) showEventDialog(m_selectedDate, id);
}

void MainWindow::deleteSelectedEvent() {
    auto items = m_eventList->selectedItems();
    if (items.isEmpty()) return;
    QString id = items.first()->data(Qt::UserRole).toString();
    if (id.isEmpty()) return;
    auto ret = QMessageBox::question(this, "Briši događaj",
        "Da li ste sigurni da želite obrisati ovaj događaj?",
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        m_events->removeEvent(id);
        refreshEventList(m_selectedDate);
    }
}

void MainWindow::onReminderTriggered(const CalendarEvent& ev) {
    m_tray->showMessage(
        "⏰  Podsjetnik — " + ev.title,
        ev.description.isEmpty() ? ev.date.toString("d.M.yyyy") + " u " + ev.time.toString("HH:mm")
                                  : ev.description,
        QSystemTrayIcon::Information, 8000);
}

void MainWindow::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    m_lblClock->setText(now.toString("dddd, d.M.yyyy  HH:mm:ss"));
}

void MainWindow::showAboutDialog() {
    QMessageBox msg(this);
    msg.setWindowTitle("O programu");
    msg.setTextFormat(Qt::RichText);
    msg.setText(R"(
<div style="font-family:Segoe UI; color:#e0e0f0;">
<h2 style="color:#9966cc;">☦ Pravoslavni Kalendar</h2>
<p style="color:#aaaacc;">Verzija 2.0 — Ultra Modern Edition</p>
<hr style="border-color:rgba(255,255,255,0.1);">
<ul style="color:#ccccee;">
  <li>Svi srpski pravoslavni praznici označeni crvenim slovima</li>
  <li>Pokretni praznici (Uskrs, Spasovdan, Duhovi...)</li>
  <li>Sistem podsjetnika za događaje</li>
  <li>Ugrađeni muzički plejer sa plejlistom</li>
  <li>Puni ekran / prozorski prikaz</li>
  <li>Sistemska ikonjica u traci</li>
</ul>
<p style="color:#666688; font-size:11px;">
  Napravljen sa ljubavlju u C++ / Qt 6<br>
  Pravoslavni kalendar koristi Julijansko računanje datuma + 13 dana (20. i 21. vijek)
</p>
</div>
)");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

// ─────────────────────────────────────────────────────────────────
void MainWindow::closeEvent(QCloseEvent *ev) {
    if (m_tray && m_tray->isVisible()) {
        hide();
        m_tray->showMessage("Pravoslavni Kalendar",
            "Program je minimizovan u sistemsku traku.",
            QSystemTrayIcon::Information, 3000);
        ev->ignore();
    } else {
        ev->accept();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev) {
    if (ev->key() == Qt::Key_F11) toggleFullScreen();
    else if (ev->key() == Qt::Key_Left  && (ev->modifiers() & Qt::ControlModifier)) prevMonth();
    else if (ev->key() == Qt::Key_Right && (ev->modifiers() & Qt::ControlModifier)) nextMonth();
    else if (ev->key() == Qt::Key_Home  && (ev->modifiers() & Qt::ControlModifier)) goToToday();
    else if (ev->key() == Qt::Key_Escape && isFullScreen()) showNormal();
    else QMainWindow::keyPressEvent(ev);
}

void MainWindow::paintEvent(QPaintEvent *ev) {
    QPainter p(this);
    p.fillRect(rect(), QColor(10, 10, 28));
    QMainWindow::paintEvent(ev);
}

void MainWindow::resizeEvent(QResizeEvent *ev) {
    QMainWindow::resizeEvent(ev);
}
