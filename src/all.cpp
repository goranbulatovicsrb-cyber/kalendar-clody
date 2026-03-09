// ═══════════════════════════════════════════════════════
// PRAVOSLAVNI KALENDAR - Fully Self-Contained Build
// Svi headeri su ugradjeni ovdje - nema ovisnosti o .h fajlovima
// ═══════════════════════════════════════════════════════

// ── Prevent individual headers from being re-included ──
#define ORTHODOXHOLIDAYS_H
#define EVENTMANAGER_H
#define MUSICPLAYER_H
#define CALENDARWIDGET_H
#define MAINWINDOW_H
#define STYLE_H

// ── Common Qt includes ──────────────────────────────────
#include <QtWidgets>
#include <QtMultimedia>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QColor>
#include <QDate>
#include <QTime>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QSettings>
#include <QUrl>
#include <QUuid>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QCheckBox>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QScreen>
#include <QColorDialog>
#include <QScrollArea>
#include <QSplitter>
#include <QtMath>

// ══════════════════════════════════════════════════════════
// INLINE HEADERS
// ══════════════════════════════════════════════════════════

// ── orthodoxholidays.h ─────────────────────────────────
struct HolidayInfo {
    QString name;
    QString shortName;
    bool isGreatFeast;
};

class OrthodoxHolidays {
public:
    static QDate calculateOrthodoxEaster(int year);
    static QMap<QDate, HolidayInfo> getHolidaysForYear(int year);
    static bool isHoliday(const QDate& date, int year);
    static bool isGreatFeast(const QDate& date, int year);
    static HolidayInfo getHolidayInfo(const QDate& date, int year);
    static QList<QPair<QDate,QDate>> getFastingPeriods(int year);
    static bool isFastingDay(const QDate& date, int year);
};

// ── eventmanager.h ────────────────────────────────────
struct CalendarEvent {
    QString id;
    QDate   date;
    QTime   time;
    QString title;
    QString description;
    bool    hasReminder;
    int     reminderMinsBefore;
    QColor  color;
};

class EventManager : public QObject {
    Q_OBJECT
public:
    explicit EventManager(QObject *parent = nullptr);
    ~EventManager();
    void addEvent(const CalendarEvent& event);
    void updateEvent(const CalendarEvent& event);
    void removeEvent(const QString& id);
    QList<CalendarEvent> eventsForDate(const QDate& date) const;
    QList<CalendarEvent> allEvents() const;
    bool hasEvents(const QDate& date) const;
    void saveAll();
    void loadAll();
signals:
    void eventsChanged(const QDate& date);
    void reminderTriggered(const CalendarEvent& event);
private slots:
    void checkReminders();
private:
    QMap<QDate, QList<CalendarEvent>> m_events;
    QTimer *m_reminderTimer;
    QString generateId() const;
};

// ── musicplayer.h ─────────────────────────────────────
class MusicPlayer : public QWidget {
    Q_OBJECT
public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer() override;
    QString currentTrackTitle() const;
    bool isPlaying() const;
    void savePlaylist();
    void loadPlaylist();
signals:
    void trackChanged(const QString& title);
protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void togglePlayPause();
    void nextTrack();
    void prevTrack();
    void openFiles();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlaybackStateChanged(int state);
    void onMediaStatusChanged(int status);
    void seekTo(int value);
    void setVolume(int value);
    void onPlaylistDoubleClicked(int row);
    void removeSelectedTrack();
    void clearPlaylist();
private:
    void setupUI();
    void playIndex(int index);
    void updatePlayButton();
    void updateTrackLabel();
    QString formatMs(qint64 ms) const;
    QString extractTitle(const QUrl& url) const;
    QMediaPlayer  *m_player;
    QAudioOutput  *m_audioOutput;
    QList<QUrl>    m_playlist;
    int   m_currentIndex = -1;
    bool  m_shuffle   = false;
    bool  m_repeat    = false;
    bool  m_autoPlay  = false;
    QPushButton *m_btnPlay, *m_btnPrev, *m_btnNext;
    QPushButton *m_btnShuffle, *m_btnRepeat, *m_btnOpen, *m_btnPlaylistToggle;
    QCheckBox   *m_chkAutoPlay;
    QSlider     *m_sliderProgress, *m_sliderVolume;
    QLabel      *m_lblTitle, *m_lblArtist, *m_lblTime, *m_lblVolIcon;
    QListWidget *m_playlistWidget;
    QFrame      *m_playlistFrame;
};

// ── calendarwidget.h ──────────────────────────────────
class CalendarWidget : public QWidget {
    Q_OBJECT
public:
    explicit CalendarWidget(EventManager *events, QWidget *parent = nullptr);
    QDate currentDate()  const { return m_currentDate; }
    QDate selectedDate() const { return m_selectedDate; }
    void  setMonth(int year, int month);
signals:
    void dateClicked(const QDate& date);
    void dateDoubleClicked(const QDate& date);
    void monthChanged(int year, int month);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    QDate dateAtPoint(const QPointF& pt) const;
    QRectF cellRect(int row, int col) const;
    void drawDayCell(QPainter& p, const QRectF& r, const QDate& date,
                     bool isCurrentMonth, bool isToday, bool isSelected,
                     bool isHovered, const HolidayInfo& holiday, bool hasEvents) const;
    void rebuildHolidays();
    EventManager *m_events;
    QDate  m_currentDate, m_selectedDate, m_hoveredDate, m_today;
    QMap<QDate, HolidayInfo> m_holidays;
    float  m_headerH = 42.0f;
    float  m_dayH    = 28.0f;
    float  cellH     = 72.0f;
    int    m_gridRows = 6;
    static const QString kDayNames[7];
};

// ── mainwindow.h ──────────────────────────────────────
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
protected:
    void closeEvent(QCloseEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
private slots:
    void prevMonth();
    void nextMonth();
    void goToToday();
    void toggleFullScreen();
    void onDateClicked(const QDate& date);
    void onDateDoubleClicked(const QDate& date);
    void onMonthChanged(int year, int month);
    void showEventDialog(const QDate& date, const QString& existingId = "");
    void refreshEventList(const QDate& date);
    void deleteSelectedEvent();
    void editSelectedEvent();
    void onReminderTriggered(const CalendarEvent& ev);
    void updateClock();
    void showAboutDialog();
private:
    void setupUI();
    void setupTrayIcon();
    void applyMonthLabel();
    EventManager   *m_events;
    CalendarWidget *m_calendar;
    MusicPlayer    *m_player;
    QSystemTrayIcon *m_tray;
    QTimer         *m_clockTimer;
    QDate           m_displayDate, m_selectedDate;
    QFrame     *m_navBar, *m_sidePanel;
    QPushButton *m_btnPrev, *m_btnNext, *m_btnToday, *m_btnFullscreen, *m_btnAbout;
    QLabel     *m_lblMonth, *m_lblClock, *m_lblHoliday;
    QLabel     *m_lblSelectedDate, *m_lblHolidayInfo;
    QListWidget *m_eventList;
    QPushButton *m_btnAddEvent, *m_btnEditEvent, *m_btnDelEvent;
};

// ── style.h inline ────────────────────────────────────
inline QString appStyleSheet() {
    return R"(
QMainWindow, QDialog { background-color: #0a0a1e; color: #e0e0f0; font-family: "Segoe UI", Arial, sans-serif; font-size: 13px; }
QWidget { color: #e0e0f0; font-family: "Segoe UI", Arial, sans-serif; }
QScrollBar:vertical { background: rgba(255,255,255,0.03); width: 8px; border-radius: 4px; }
QScrollBar::handle:vertical { background: rgba(255,255,255,0.15); border-radius: 4px; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.28); }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal { background: rgba(255,255,255,0.03); height: 8px; border-radius: 4px; }
QScrollBar::handle:horizontal { background: rgba(255,255,255,0.15); border-radius: 4px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
QToolTip { background: #1a1a3e; color: #e0e0f0; border: 1px solid rgba(255,255,255,0.12); border-radius: 6px; padding: 4px 8px; }
QLabel { background: transparent; }
QLineEdit, QTextEdit, QPlainTextEdit { background: rgba(255,255,255,0.06); border: 1px solid rgba(255,255,255,0.12); border-radius: 8px; padding: 6px 10px; color: #e0e0f0; selection-background-color: #533483; }
QLineEdit:focus, QTextEdit:focus { border: 1px solid #533483; background: rgba(83,52,131,0.15); }
QComboBox { background: rgba(255,255,255,0.07); border: 1px solid rgba(255,255,255,0.12); border-radius: 8px; padding: 5px 10px; color: #e0e0f0; }
QComboBox:hover { border-color: #533483; }
QComboBox QAbstractItemView { background: #12122e; border: 1px solid rgba(255,255,255,0.12); border-radius: 8px; selection-background-color: #533483; color: #e0e0f0; }
QCheckBox { spacing: 8px; }
QCheckBox::indicator { width: 16px; height: 16px; border: 1px solid rgba(255,255,255,0.2); border-radius: 4px; background: rgba(255,255,255,0.05); }
QCheckBox::indicator:checked { background: #533483; border-color: #7b5ea7; }
QSpinBox, QTimeEdit, QDateEdit { background: rgba(255,255,255,0.07); border: 1px solid rgba(255,255,255,0.12); border-radius: 8px; padding: 5px 10px; color: #e0e0f0; }
QSpinBox:focus, QTimeEdit:focus { border-color: #533483; }
QSpinBox::up-button, QSpinBox::down-button, QTimeEdit::up-button, QTimeEdit::down-button { background: rgba(255,255,255,0.08); border: none; width: 18px; border-radius: 4px; }
QGroupBox { border: 1px solid rgba(255,255,255,0.1); border-radius: 10px; margin-top: 16px; padding: 12px 8px 8px 8px; font-weight: 600; color: #aaaacc; font-size: 12px; }
QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 8px; color: #aaaacc; }
QMessageBox { background: #12122e; color: #e0e0f0; }
QMessageBox QPushButton { background: rgba(83,52,131,0.5); border: 1px solid #533483; border-radius: 8px; color: #e0d0ff; padding: 6px 18px; min-width: 70px; }
QMessageBox QPushButton:hover { background: rgba(83,52,131,0.8); }
QSplitter::handle { background: rgba(255,255,255,0.06); }
QSplitter::handle:horizontal { width: 1px; }
QSplitter::handle:vertical { height: 1px; }
)";
}

// ══════════════════════════════════════════════════════════
// IMPLEMENTATIONS
// ══════════════════════════════════════════════════════════

#include <QList>

QDate OrthodoxHolidays::calculateOrthodoxEaster(int year) {
    int a = year % 4;
    int b = year % 7;
    int c = year % 19;
    int d = (19 * c + 15) % 30;
    int e = (2 * a + 4 * b - d + 34) % 7;
    int month = (d + e + 114) / 31;
    int day   = (d + e + 114) % 31 + 1;
    QDate julianEaster(year, month, day);
    return julianEaster.addDays(13);
}

QMap<QDate, HolidayInfo> OrthodoxHolidays::getHolidaysForYear(int year) {
    QMap<QDate, HolidayInfo> map;

    auto add = [&](int m, int d, const QString& name, const QString& shortName, bool great) {
        map.insert(QDate(year, m, d), {name, shortName, great});
    };

    // ══ JANUAR ══════════════════════════════════════════════════════════
    add(1,  7,  "Рођење Господа Исуса Христа — Божић",              "Božić",             true);
    add(1,  8,  "Сабор Пресвете Богородице",                          "Sabor Bogородице",  false);
    add(1,  9,  "Свети Стефан Првомученик",                           "Sv. Stefan",        true);
    add(1, 13,  "Свети Мелентије епископ",                            "Sv. Melentije",     false);
    add(1, 14,  "Обрезање Господње — Свети Василије Велики",          "Sv. Vasilije",      true);
    add(1, 17,  "Свети Антоније Велики",                              "Sv. Antonije",      false);
    add(1, 19,  "Богојављење — Крштење Господа Исуса Христа",         "Bogojavljenje",     true);
    add(1, 20,  "Сабор Светог Јована Крститеља",                      "Sv. Jovan Krstitelj", false);
    add(1, 27,  "Свети Сава Српски — Просветитељ",                    "Sveti Sava",        true);
    add(1, 30,  "Свети Три Јерарха",                                   "Tri Jerarha",       false);

    // ══ FEBRUAR ══════════════════════════════════════════════════════════
    add(2,  3,  "Свети Симеон Богопримац и Ана Пророчица",            "Sv. Simeon",        false);
    add(2, 10,  "Свети мученик Харалампије",                           "Sv. Haralampije",   false);
    add(2, 12,  "Свети Мелентије Антиохијски",                         "Sv. Melentije",     false);
    add(2, 14,  "Свети Трифун мученик",                                "Sv. Trifun",        false);
    add(2, 15,  "Сретење Господње — Дан државности Србије",           "Sretenje",          true);

    // ══ MART ══════════════════════════════════════════════════════════════
    add(3,  9,  "Свети Тарасије архиепископ Цариградски",              "Sv. Tarasije",      false);
    add(3, 22,  "Свети 40 Мученика Севастијских",                      "40 Mučenika",       true);
    add(3, 25,  "Благовести — Благовештење Пресвете Богородице (НС)", "Blagovesti (NS)",   false);

    // ══ APRIL ══════════════════════════════════════════════════════════════
    add(4,  7,  "Благовести — Благовештење Пресвете Богородице",      "Blagovesti",        true);
    add(4, 23,  "Свети апостол и jeванђелист Марко",                   "Sv. Marko",         false);
    add(4, 30,  "Свети апостол Јаков Зеведејев",                       "Sv. Jakov",         false);

    // ══ MAJ ══════════════════════════════════════════════════════════════
    add(5,  6,  "Свети Јов Многострадалец — Ђурђевдан",               "Đurđevdan",         true);
    add(5,  9,  "Свети пророк Исаија",                                 "Sv. Isaija",        false);
    add(5, 12,  "Свети Василије Острошки — Чудотворац",                "Sv. Vasilije Ostroški", true);
    add(5, 21,  "Свети Цар Константин и царица Јелена",                "Sv. Konstantin i Jelena", true);
    add(5, 22,  "Свети Никола Чудотворац — Пренос моштију",            "Sv. Nikola (prenos)", false);

    // ══ JUN ══════════════════════════════════════════════════════════════
    add(6,  2,  "Свети mученик Талалеј",                               "Sv. Talалеј",       false);
    add(6, 11,  "Свети Лука Кримски — Архиепископ и исцелитељ",        "Sv. Luka Krimski",  true);
    add(6, 13,  "Свети Апостол Варнава",                               "Sv. Varnava",       false);
    add(6, 24,  "Рођење Светог Јована Крститеља — Ивандан",            "Ivandan",           true);
    add(6, 28,  "Видовдан — Свети кнез Лазар",                         "Vidovdan",          true);
    add(6, 29,  "Свети апостоли Петар и Павле — Петровдан",            "Petrovdan",         true);

    // ══ JUL ══════════════════════════════════════════════════════════════
    add(7, 12,  "Прва и Друга налажење главе Светог Јована Крститеља", "Nalaženje glave",   false);
    add(7, 13,  "Сабор 12 апостола",                                   "Sabor 12 apostola", false);
    add(7, 14,  "Свети бесребреници Козма и Дамјан",                   "Sv. Kozma i Damjan",false);
    add(7, 20,  "Света равноапостолна Марија Магдалина",               "Sv. Magdalina",     false);
    add(7, 30,  "Света великомученица Марина",                          "Sv. Marina",        true);

    // ══ AVGUST ══════════════════════════════════════════════════════════
    add(8,  2,  "Свети пророк Илија — Илиндан",                        "Ilindan",           true);
    add(8,  9,  "Свети великомученик Пантелејмон — Панталија",         "Sv. Pantelejmon",   true);
    add(8, 16,  "Пренос нерукотвореног образа Господа Исуса Христа",   "Ubrus",             false);
    add(8, 19,  "Преображење Господа Исуса Христа",                    "Preobraženje",      true);
    add(8, 25,  "Свети Варталомеј апостол",                             "Sv. Vartolomej",    false);
    add(8, 28,  "Успење Пресвете Богородице — Велика Госпојина",        "Vel. Gospojina",    true);
    add(8, 29,  "Усековање главе Светог Јована Крститеља",              "Usekovanje",        true);

    // ══ SEPTEMBAR ══════════════════════════════════════════════════════
    add(9, 11,  "Усековање главе Светог Јована Крститеља (по новом)",  "Usekovanje (NS)",   false);
    add(9, 14,  "Воздвижење Часног и Животворног Крста",               "Vozdviženje",       true);
    add(9, 21,  "Рођење Пресвете Богородице — Мала Госпојина",          "Mala Gospojina",    true);
    add(9, 27,  "Свети Апостол и јеванђелист Јован Богослов",           "Sv. Jovan Bogosl.", false);

    // ══ OKTOBAR ══════════════════════════════════════════════════════════
    add(10, 14, "Покров Пресвете Богородице",                           "Pokrov",            true);
    add(10, 19, "Свети апостол Тома",                                   "Sv. Toma",          false);
    add(10, 26, "Свети великомученик Димитрије Солунски — Митровдан",   "Mitrovdan",         true);
    add(10, 27, "Света Петка Параскева — Петковдан",                    "Sv. Petka",         true);
    add(10, 31, "Свети апостол и јеванђелист Лука",                     "Sv. Luka apostol",  true);

    // ══ NOVEMBAR ══════════════════════════════════════════════════════════
    add(11,  8, "Свети великомученик Димитрије — Митровдан (по ст.)", "Mitrovdan (st.)",   false);
    add(11, 14, "Свети бесребреници Козма и Дамјан",                   "Sv. Kozma i Damjan",false);
    add(11, 21, "Аранђеловдан — Сабор Светих Арханђела Михаила и Гаврила", "Aranđelovdan", true);
    add(11, 26, "Свети Јован Златоуст",                                 "Sv. Jovan Zlatoust",false);
    add(11, 28, "Свети мученик Стефан Дечански",                        "Sv. Stefan Dečanski", false);

    // ══ DECEMBAR ══════════════════════════════════════════════════════════
    add(12,  4, "Ваведење Пресвете Богородице у храм",                  "Vavedenje",         true);
    add(12,  9, "Зачеће Пресвете Богородице — Света Ана",               "Sv. Ana",           false);
    add(12, 13, "Свети Апостол Андрија Првозвани — Андрићевдан",        "Sv. Andrija",       true);
    add(12, 17, "Свети пророк Данило",                                   "Sv. Danilo",        false);
    add(12, 19, "Свети Никола Чудотворац — Николиндан",                 "Nikolindan",        true);
    add(12, 24, "Бадње Вече",                                            "Badnje veče",       true);
    add(12, 25, "Свети Спиридон Тримитунтски",                          "Sv. Spiridon",      false);

    // ══ POKRETNI PRAZNICI ══════════════════════════════════════════════
    QDate easter = calculateOrthodoxEaster(year);

    map.insert(easter.addDays(-48), {"Почетак Великог поста — Чисти понедјељак", "Čisti ponedeljak", false});
    map.insert(easter.addDays(-14), {"Лазарева субота", "Lazareva sub.", false});
    map.insert(easter.addDays(-7),  {"Цвети — Улазак Господа у Јерусалим", "Cveti", true});
    map.insert(easter.addDays(-3),  {"Велики четвртак — Тајна вечера", "Vel. Četvrtak", false});
    map.insert(easter.addDays(-2),  {"Велики петак — Распеће Господа", "Vel. Petak", true});
    map.insert(easter.addDays(-1),  {"Велика субота", "Vel. Subota", false});
    map.insert(easter,              {"Васкрсење Господа Исуса Христа — Ускрс ☦", "Uskrs ☦", true});
    map.insert(easter.addDays(1),   {"Свјетли понедјељак", "Svj. ponedeljak", false});
    map.insert(easter.addDays(39),  {"Вазнесење Господа — Спасовдан", "Spasovdan", true});
    map.insert(easter.addDays(49),  {"Силазак Светог Духа — Духови / Тројице", "Duhovi", true});
    map.insert(easter.addDays(56),  {"Недеља Свих Светих", "Sv. Sveti", false});

    return map;
}

bool OrthodoxHolidays::isHoliday(const QDate& date, int year) {
    return getHolidaysForYear(year).contains(date);
}

bool OrthodoxHolidays::isGreatFeast(const QDate& date, int year) {
    auto map = getHolidaysForYear(year);
    if (map.contains(date)) return map[date].isGreatFeast;
    return false;
}

HolidayInfo OrthodoxHolidays::getHolidayInfo(const QDate& date, int year) {
    auto map = getHolidaysForYear(year);
    if (map.contains(date)) return map[date];
    return {"", "", false};
}

QList<QPair<QDate,QDate>> OrthodoxHolidays::getFastingPeriods(int year) {
    QList<QPair<QDate,QDate>> fasts;
    QDate easter = calculateOrthodoxEaster(year);
    fasts.append({easter.addDays(-48), easter.addDays(-1)});
    fasts.append({easter.addDays(57), QDate(year, 6, 28)});
    fasts.append({QDate(year, 8, 1), QDate(year, 8, 13)});
    fasts.append({QDate(year, 11, 15), QDate(year, 12, 31)});
    return fasts;
}

bool OrthodoxHolidays::isFastingDay(const QDate& date, int year) {
    if (date.dayOfWeek() == 3 || date.dayOfWeek() == 5) return true;
    for (const auto& period : getFastingPeriods(year))
        if (date >= period.first && date <= period.second) return true;
    return false;
}
#include <QSettings>
#include <QDateTime>
#include <QUuid>
#include <QColor>

EventManager::EventManager(QObject *parent) : QObject(parent) {
    m_reminderTimer = new QTimer(this);
    m_reminderTimer->setInterval(30000); // check every 30 seconds
    connect(m_reminderTimer, &QTimer::timeout, this, &EventManager::checkReminders);
    m_reminderTimer->start();
    loadAll();
}

EventManager::~EventManager() {
    saveAll();
}

QString EventManager::generateId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

void EventManager::addEvent(const CalendarEvent& event) {
    CalendarEvent ev = event;
    if (ev.id.isEmpty()) ev.id = generateId();
    if (!ev.color.isValid()) ev.color = QColor(83, 52, 131); // default purple
    m_events[ev.date].append(ev);
    saveAll();
    emit eventsChanged(ev.date);
}

void EventManager::updateEvent(const CalendarEvent& event) {
    for (auto it = m_events.begin(); it != m_events.end(); ++it) {
        auto& list = it.value();
        for (int i = 0; i < list.size(); ++i) {
            if (list[i].id == event.id) {
                list.removeAt(i);
                break;
            }
        }
        if (list.isEmpty()) {
            it = m_events.erase(it);
            break;
        }
    }
    addEvent(event);
}

void EventManager::removeEvent(const QString& id) {
    QDate changedDate;
    for (auto it = m_events.begin(); it != m_events.end(); ++it) {
        auto& list = it.value();
        for (int i = 0; i < list.size(); ++i) {
            if (list[i].id == id) {
                changedDate = list[i].date;
                list.removeAt(i);
                break;
            }
        }
        if (list.isEmpty()) {
            m_events.erase(it);
            break;
        }
    }
    saveAll();
    if (changedDate.isValid()) emit eventsChanged(changedDate);
}

QList<CalendarEvent> EventManager::eventsForDate(const QDate& date) const {
    return m_events.value(date);
}

QList<CalendarEvent> EventManager::allEvents() const {
    QList<CalendarEvent> all;
    for (const auto& list : m_events)
        all.append(list);
    return all;
}

bool EventManager::hasEvents(const QDate& date) const {
    return m_events.contains(date) && !m_events[date].isEmpty();
}

void EventManager::saveAll() {
    QSettings settings("OrthodoxCalendar", "PravoslavniKalendar");
    settings.remove("events");
    settings.beginWriteArray("events");
    int idx = 0;
    for (const auto& list : m_events) {
        for (const auto& ev : list) {
            settings.setArrayIndex(idx++);
            settings.setValue("id",          ev.id);
            settings.setValue("date",         ev.date.toString(Qt::ISODate));
            settings.setValue("time",         ev.time.toString("HH:mm"));
            settings.setValue("title",        ev.title);
            settings.setValue("description",  ev.description);
            settings.setValue("hasReminder",  ev.hasReminder);
            settings.setValue("reminderMins", ev.reminderMinsBefore);
            settings.setValue("color",        ev.color.name());
        }
    }
    settings.endArray();
}

void EventManager::loadAll() {
    m_events.clear();
    QSettings settings("OrthodoxCalendar", "PravoslavniKalendar");
    int count = settings.beginReadArray("events");
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        CalendarEvent ev;
        ev.id                  = settings.value("id").toString();
        ev.date                = QDate::fromString(settings.value("date").toString(), Qt::ISODate);
        ev.time                = QTime::fromString(settings.value("time").toString(), "HH:mm");
        ev.title               = settings.value("title").toString();
        ev.description         = settings.value("description").toString();
        ev.hasReminder         = settings.value("hasReminder", false).toBool();
        ev.reminderMinsBefore  = settings.value("reminderMins", 15).toInt();
        ev.color               = QColor(settings.value("color", "#533483").toString());
        if (ev.date.isValid() && !ev.title.isEmpty())
            m_events[ev.date].append(ev);
    }
    settings.endArray();
}

void EventManager::checkReminders() {
    QDateTime now = QDateTime::currentDateTime();
    for (const auto& list : m_events) {
        for (const auto& ev : list) {
            if (!ev.hasReminder) continue;
            QDateTime evTime(ev.date, ev.time);
            qint64 diffSecs = now.secsTo(evTime);
            qint64 reminderSecs = ev.reminderMinsBefore * 60;
            // Trigger if within 30 seconds of reminder time
            if (diffSecs >= 0 && diffSecs <= reminderSecs && diffSecs > reminderSecs - 30) {
                emit reminderTriggered(ev);
            }
        }
    }
}
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QStandardPaths>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QTime>
#include <QSizePolicy>
#include <QFont>
#include <QColor>
#include <QCheckBox>
#include <QSettings>
#include <QTimer>

static const char* BTN_STYLE = R"(
QPushButton {
    background: transparent;
    border: none;
    color: #b0b0d0;
    font-size: 18px;
    padding: 4px;
    border-radius: 6px;
}
QPushButton:hover {
    background: rgba(255,255,255,0.08);
    color: #ffffff;
}
QPushButton:pressed { background: rgba(255,255,255,0.15); }
QPushButton:checked { color: #e94560; }
)";

static const char* SLIDER_STYLE = R"(
QSlider::groove:horizontal {
    height: 4px;
    background: rgba(255,255,255,0.12);
    border-radius: 2px;
}
QSlider::sub-page:horizontal {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #533483, stop:1 #e94560);
    border-radius: 2px;
}
QSlider::handle:horizontal {
    width: 12px; height: 12px;
    background: #e94560;
    border-radius: 6px;
    margin: -4px 0;
}
QSlider::handle:horizontal:hover { background: #ff6b84; }
QSlider::groove:vertical {
    width: 4px;
    background: rgba(255,255,255,0.12);
    border-radius: 2px;
}
QSlider::sub-page:vertical {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #533483, stop:1 #e94560);
    border-radius: 2px;
}
QSlider::handle:vertical {
    width: 12px; height: 12px;
    background: #7b5ea7;
    border-radius: 6px;
    margin: 0 -4px;
}
)";

MusicPlayer::MusicPlayer(QWidget *parent) : QWidget(parent) {
    m_player = new QMediaPlayer(this);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.7f);
    connect(m_player, &QMediaPlayer::positionChanged,   this, &MusicPlayer::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,   this, &MusicPlayer::onDurationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, [this](QMediaPlayer::PlaybackState s){ onPlaybackStateChanged((int)s); });
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus s){ onMediaStatusChanged((int)s); });
#else
    m_player->setVolume(70);
    connect(m_player, &QMediaPlayer::positionChanged,   this, &MusicPlayer::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,   this, &MusicPlayer::onDurationChanged);
    connect(m_player, QOverload<>::of(&QMediaPlayer::stateChanged),
            this, [this]{ onPlaybackStateChanged((int)m_player->state()); });
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus s){ onMediaStatusChanged((int)s); });
#endif
    setupUI();
    loadPlaylist();

    // Auto-play after short delay so UI loads first
    if (m_autoPlay && !m_playlist.isEmpty()) {
        QTimer::singleShot(800, this, [this]{
            playIndex(m_currentIndex >= 0 ? m_currentIndex : 0);
        });
    }
}

MusicPlayer::~MusicPlayer() {
    savePlaylist();
}

void MusicPlayer::savePlaylist() {
    QSettings s("OrthodoxCalendar", "PravoslavniKalendar");
    s.beginWriteArray("playlist");
    for (int i = 0; i < m_playlist.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue("url", m_playlist[i].toString());
    }
    s.endArray();
    s.setValue("playlist_index",    m_currentIndex);
    s.setValue("playlist_shuffle",  m_shuffle);
    s.setValue("playlist_repeat",   m_repeat);
    s.setValue("playlist_autoplay", m_autoPlay);
    s.setValue("player_volume",     m_sliderVolume->value());
}

void MusicPlayer::loadPlaylist() {
    QSettings s("OrthodoxCalendar", "PravoslavniKalendar");
    m_shuffle  = s.value("playlist_shuffle",  false).toBool();
    m_repeat   = s.value("playlist_repeat",   false).toBool();
    m_autoPlay = s.value("playlist_autoplay", false).toBool();
    int vol    = s.value("player_volume", 70).toInt();
    int savedIndex = s.value("playlist_index", 0).toInt();

    m_btnShuffle->setChecked(m_shuffle);
    m_btnRepeat->setChecked(m_repeat);
    m_chkAutoPlay->setChecked(m_autoPlay);
    m_sliderVolume->setValue(vol);

    int count = s.beginReadArray("playlist");
    for (int i = 0; i < count; ++i) {
        s.setArrayIndex(i);
        QUrl url(s.value("url").toString());
        // Only add if file still exists
        if (url.isLocalFile() && QFileInfo::exists(url.toLocalFile())) {
            m_playlist.append(url);
            m_playlistWidget->addItem("♪  " + extractTitle(url));
        }
    }
    s.endArray();

    if (!m_playlist.isEmpty()) {
        m_currentIndex = qBound(0, savedIndex, m_playlist.size() - 1);
        updateTrackLabel();
        m_playlistWidget->setCurrentRow(m_currentIndex);
    }
}

void MusicPlayer::setupUI() {
    setMinimumHeight(140);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 8, 16, 8);
    mainLayout->setSpacing(6);

    // Track info row
    auto *infoRow = new QHBoxLayout();
    m_lblTitle = new QLabel("♪  Nema pjesme", this);
    m_lblTitle->setStyleSheet("color:#eaeaea; font-size:14px; font-weight:600;");
    m_lblTitle->setMaximumWidth(400);
    m_lblTitle->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_lblArtist = new QLabel("", this);
    m_lblArtist->setStyleSheet("color:#8888aa; font-size:11px;");
    auto *titCol = new QVBoxLayout();
    titCol->setSpacing(1);
    titCol->addWidget(m_lblTitle);
    titCol->addWidget(m_lblArtist);
    infoRow->addLayout(titCol);
    infoRow->addStretch();

    // AutoPlay checkbox
    m_chkAutoPlay = new QCheckBox("▶ Auto-play", this);
    m_chkAutoPlay->setToolTip("Automatski pokreni muziku pri pokretanju programa");
    m_chkAutoPlay->setStyleSheet(R"(
        QCheckBox { color: #8888aa; font-size: 11px; spacing: 5px; }
        QCheckBox:hover { color: #aaaacc; }
        QCheckBox::indicator {
            width: 14px; height: 14px;
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 3px;
            background: rgba(255,255,255,0.05);
        }
        QCheckBox::indicator:checked {
            background: #e94560;
            border-color: #e94560;
        }
    )");
    connect(m_chkAutoPlay, &QCheckBox::toggled, this, [this](bool c){
        m_autoPlay = c;
        savePlaylist();
    });
    infoRow->addWidget(m_chkAutoPlay);

    // Volume
    m_lblVolIcon = new QLabel("🔊", this);
    m_lblVolIcon->setStyleSheet("font-size:16px;");
    m_sliderVolume = new QSlider(Qt::Horizontal, this);
    m_sliderVolume->setRange(0, 100);
    m_sliderVolume->setValue(70);
    m_sliderVolume->setMaximumWidth(90);
    m_sliderVolume->setStyleSheet(SLIDER_STYLE);
    connect(m_sliderVolume, &QSlider::valueChanged, this, &MusicPlayer::setVolume);
    infoRow->addWidget(m_lblVolIcon);
    infoRow->addWidget(m_sliderVolume);
    mainLayout->addLayout(infoRow);

    // Progress bar
    auto *progressRow = new QHBoxLayout();
    m_sliderProgress = new QSlider(Qt::Horizontal, this);
    m_sliderProgress->setRange(0, 0);
    m_sliderProgress->setStyleSheet(SLIDER_STYLE);
    connect(m_sliderProgress, &QSlider::sliderMoved, this, &MusicPlayer::seekTo);
    m_lblTime = new QLabel("0:00 / 0:00", this);
    m_lblTime->setStyleSheet("color:#666688; font-size:11px; min-width:90px;");
    m_lblTime->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    progressRow->addWidget(m_sliderProgress, 1);
    progressRow->addWidget(m_lblTime);
    mainLayout->addLayout(progressRow);

    // Controls row
    auto *controlsRow = new QHBoxLayout();
    controlsRow->setSpacing(4);
    auto makeBtn = [&](const QString& text, const QString& tip) -> QPushButton* {
        auto *b = new QPushButton(text, this);
        b->setToolTip(tip);
        b->setStyleSheet(BTN_STYLE);
        b->setFixedSize(36, 36);
        return b;
    };
    m_btnShuffle = makeBtn("⇄", "Slučajni raspored");
    m_btnShuffle->setCheckable(true);
    m_btnRepeat  = makeBtn("↻", "Ponavljaj");
    m_btnRepeat->setCheckable(true);
    m_btnPrev    = makeBtn("⏮", "Prethodna");
    m_btnPlay    = makeBtn("▶", "Reprodukuj / Pauza");
    m_btnPlay->setStyleSheet(QString(BTN_STYLE) + "QPushButton { font-size:22px; color:#e94560; }");
    m_btnNext    = makeBtn("⏭", "Sljedeća");
    m_btnOpen    = makeBtn("📂", "Dodaj muziku");
    m_btnPlaylistToggle = makeBtn("☰", "Prikaži listu");
    m_btnPlaylistToggle->setCheckable(true);

    controlsRow->addWidget(m_btnShuffle);
    controlsRow->addWidget(m_btnRepeat);
    controlsRow->addStretch();
    controlsRow->addWidget(m_btnPrev);
    controlsRow->addWidget(m_btnPlay);
    controlsRow->addWidget(m_btnNext);
    controlsRow->addStretch();
    controlsRow->addWidget(m_btnOpen);
    controlsRow->addWidget(m_btnPlaylistToggle);
    mainLayout->addLayout(controlsRow);

    // Playlist panel
    m_playlistFrame = new QFrame(this);
    m_playlistFrame->setVisible(false);
    m_playlistFrame->setStyleSheet(
        "QFrame { background: rgba(10,10,30,0.6); border-radius:8px; border:1px solid rgba(255,255,255,0.07); }");
    auto *plLayout = new QVBoxLayout(m_playlistFrame);
    plLayout->setContentsMargins(8, 6, 8, 6);
    plLayout->setSpacing(4);
    auto *plHeader = new QHBoxLayout();
    auto *plTitle = new QLabel("🎵  Plejlista", m_playlistFrame);
    plTitle->setStyleSheet("color:#aaa; font-size:12px; font-weight:600;");
    auto *btnRemove = new QPushButton("Ukloni", m_playlistFrame);
    btnRemove->setStyleSheet(
        "QPushButton{background:rgba(233,69,96,0.15);color:#e94560;"
        "border:1px solid #e94560;border-radius:4px;padding:2px 8px;font-size:11px;}"
        "QPushButton:hover{background:rgba(233,69,96,0.3);}");
    auto *btnClear = new QPushButton("Očisti sve", m_playlistFrame);
    btnClear->setStyleSheet(
        "QPushButton{background:rgba(255,255,255,0.05);color:#888;"
        "border:1px solid rgba(255,255,255,0.1);border-radius:4px;padding:2px 8px;font-size:11px;}"
        "QPushButton:hover{background:rgba(255,255,255,0.1);}");
    plHeader->addWidget(plTitle);
    plHeader->addStretch();
    plHeader->addWidget(btnRemove);
    plHeader->addWidget(btnClear);
    plLayout->addLayout(plHeader);
    m_playlistWidget = new QListWidget(m_playlistFrame);
    m_playlistWidget->setMaximumHeight(140);
    m_playlistWidget->setStyleSheet(
        "QListWidget { background:transparent; border:none; color:#ccc; font-size:12px; }"
        "QListWidget::item { padding:3px 6px; border-radius:4px; }"
        "QListWidget::item:hover { background:rgba(255,255,255,0.06); }"
        "QListWidget::item:selected { background:rgba(233,69,96,0.2); color:#fff; }");
    plLayout->addWidget(m_playlistWidget);
    mainLayout->addWidget(m_playlistFrame);

    // Connections
    connect(m_btnPlay,  &QPushButton::clicked, this, &MusicPlayer::togglePlayPause);
    connect(m_btnPrev,  &QPushButton::clicked, this, &MusicPlayer::prevTrack);
    connect(m_btnNext,  &QPushButton::clicked, this, &MusicPlayer::nextTrack);
    connect(m_btnOpen,  &QPushButton::clicked, this, &MusicPlayer::openFiles);
    connect(m_btnShuffle, &QPushButton::toggled, this, [this](bool c){ m_shuffle = c; savePlaylist(); });
    connect(m_btnRepeat,  &QPushButton::toggled, this, [this](bool c){ m_repeat  = c; savePlaylist(); });
    connect(m_btnPlaylistToggle, &QPushButton::toggled, m_playlistFrame, &QFrame::setVisible);
    connect(m_playlistWidget, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*){ onPlaylistDoubleClicked(m_playlistWidget->currentRow()); });
    connect(btnRemove, &QPushButton::clicked, this, &MusicPlayer::removeSelectedTrack);
    connect(btnClear,  &QPushButton::clicked, this, &MusicPlayer::clearPlaylist);
}

void MusicPlayer::playIndex(int index) {
    if (index < 0 || index >= m_playlist.size()) return;
    m_currentIndex = index;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_player->setSource(m_playlist[index]);
#else
    m_player->setMedia(m_playlist[index]);
#endif
    m_player->play();
    updateTrackLabel();
    m_playlistWidget->setCurrentRow(index);
    for (int i = 0; i < m_playlistWidget->count(); ++i) {
        auto *item = m_playlistWidget->item(i);
        item->setForeground(i == index ? QColor("#e94560") : QColor("#cccccc"));
        QFont f = item->font();
        f.setBold(i == index);
        item->setFont(f);
    }
    emit trackChanged(extractTitle(m_playlist[index]));
}

void MusicPlayer::togglePlayPause() {
    if (m_playlist.isEmpty()) { openFiles(); return; }
    if (m_currentIndex < 0) { playIndex(0); return; }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (m_player->playbackState() == QMediaPlayer::PlayingState)
        m_player->pause();
    else
        m_player->play();
#else
    if (m_player->state() == QMediaPlayer::PlayingState)
        m_player->pause();
    else
        m_player->play();
#endif
    updatePlayButton();
}

void MusicPlayer::nextTrack() {
    if (m_playlist.isEmpty()) return;
    int next = m_shuffle
        ? QRandomGenerator::global()->bounded(m_playlist.size())
        : (m_currentIndex + 1) % m_playlist.size();
    playIndex(next);
}

void MusicPlayer::prevTrack() {
    if (m_playlist.isEmpty()) return;
    playIndex((m_currentIndex - 1 + m_playlist.size()) % m_playlist.size());
}

void MusicPlayer::openFiles() {
    QStringList files = QFileDialog::getOpenFileNames(
        this, "Dodaj muziku u plejlistu",
        QStandardPaths::writableLocation(QStandardPaths::MusicLocation),
        "Audio fajlovi (*.mp3 *.flac *.wav *.ogg *.aac *.m4a *.wma *.opus)");
    for (const auto& f : files) {
        QUrl url = QUrl::fromLocalFile(f);
        m_playlist.append(url);
        m_playlistWidget->addItem("♪  " + extractTitle(url));
    }
    if (m_currentIndex < 0 && !m_playlist.isEmpty())
        playIndex(0);
    savePlaylist();
}

void MusicPlayer::seekTo(int value)   { m_player->setPosition((qint64)value); }

void MusicPlayer::setVolume(int value) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioOutput->setVolume(value / 100.0f);
#else
    m_player->setVolume(value);
#endif
    m_lblVolIcon->setText(value == 0 ? "🔇" : value < 40 ? "🔉" : "🔊");
}

void MusicPlayer::onPositionChanged(qint64 pos) {
    m_sliderProgress->setValue((int)pos);
    m_lblTime->setText(formatMs(pos) + " / " + formatMs(m_player->duration()));
}

void MusicPlayer::onDurationChanged(qint64 dur) {
    m_sliderProgress->setRange(0, (int)dur);
}

void MusicPlayer::onPlaybackStateChanged(int) { updatePlayButton(); }

void MusicPlayer::onMediaStatusChanged(int status) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (status == (int)QMediaPlayer::EndOfMedia) {
#else
    if (status == QMediaPlayer::EndOfMedia) {
#endif
        if (m_repeat) { m_player->setPosition(0); m_player->play(); }
        else nextTrack();
    }
}

void MusicPlayer::onPlaylistDoubleClicked(int row) { playIndex(row); }

void MusicPlayer::removeSelectedTrack() {
    int row = m_playlistWidget->currentRow();
    if (row < 0 || row >= m_playlist.size()) return;
    m_playlist.removeAt(row);
    delete m_playlistWidget->takeItem(row);
    if (m_currentIndex == row) { m_currentIndex = -1; m_player->stop(); updateTrackLabel(); }
    else if (m_currentIndex > row) m_currentIndex--;
    savePlaylist();
}

void MusicPlayer::clearPlaylist() {
    m_playlist.clear();
    m_playlistWidget->clear();
    m_currentIndex = -1;
    m_player->stop();
    updateTrackLabel();
    savePlaylist();
}

void MusicPlayer::updatePlayButton() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool playing = (m_player->playbackState() == QMediaPlayer::PlayingState);
#else
    bool playing = (m_player->state() == QMediaPlayer::PlayingState);
#endif
    m_btnPlay->setText(playing ? "⏸" : "▶");
}

void MusicPlayer::updateTrackLabel() {
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size()) {
        m_lblTitle->setText("♪  Nema pjesme");
        m_lblArtist->setText("");
    } else {
        m_lblTitle->setText("♪  " + extractTitle(m_playlist[m_currentIndex]));
        m_lblArtist->setText(QString("Pjesma %1 od %2")
            .arg(m_currentIndex + 1).arg(m_playlist.size()));
    }
}

QString MusicPlayer::formatMs(qint64 ms) const {
    if (ms <= 0) return "0:00";
    int secs = (int)(ms / 1000), mins = secs / 60; secs %= 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}

QString MusicPlayer::extractTitle(const QUrl& url) const {
    return QFileInfo(url.toLocalFile()).completeBaseName();
}

bool MusicPlayer::isPlaying() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return m_player->playbackState() == QMediaPlayer::PlayingState;
#else
    return m_player->state() == QMediaPlayer::PlayingState;
#endif
}

QString MusicPlayer::currentTrackTitle() const {
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size()) return "";
    return extractTitle(m_playlist[m_currentIndex]);
}

void MusicPlayer::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1,1,-1,-1), 16, 16);
    p.fillPath(path, QColor(12, 12, 35, 200));
    p.setPen(QPen(QColor(255,255,255,18), 1));
    p.drawPath(path);
    QLinearGradient grad(0,0,0,30);
    grad.setColorAt(0, QColor(83,52,131,40));
    grad.setColorAt(1, Qt::transparent);
    p.fillPath(path, grad);
}
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
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[]) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("PravoslavniKalendar");
    app.setApplicationDisplayName("Pravoslavni Kalendar");
    app.setOrganizationName("OrthodoxCalendar");
    app.setOrganizationDomain("orthodox-calendar.app");
    app.setApplicationVersion("2.0");

    // Set default font
    QFont appFont("Segoe UI", 10);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(appFont);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "all.moc"
