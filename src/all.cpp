// ═══════════════════════════════════════════════════════
// PRAVOSLAVNI KALENDAR - Single File Build
// Sav kod je u jednom fajlu - nema dependency problema
// ═══════════════════════════════════════════════════════

#include "orthodoxholidays.h"
#include <QList>

// Calculate Orthodox Easter using Julian calendar algorithm
// Returns Gregorian date (Julian + 13 days for 20th/21st century)
QDate OrthodoxHolidays::calculateOrthodoxEaster(int year) {
    int a = year % 4;
    int b = year % 7;
    int c = year % 19;
    int d = (19 * c + 15) % 30;
    int e = (2 * a + 4 * b - d + 34) % 7;
    int month = (d + e + 114) / 31;
    int day   = (d + e + 114) % 31 + 1;
    // Julian date -> Gregorian: add 13 days (valid 1900-2099)
    QDate julianEaster(year, month, day);
    return julianEaster.addDays(13);
}

QMap<QDate, HolidayInfo> OrthodoxHolidays::getHolidaysForYear(int year) {
    QMap<QDate, HolidayInfo> map;

    auto add = [&](int m, int d, const QString& name, const QString& shortName, bool great) {
        map.insert(QDate(year, m, d), {name, shortName, great});
    };

    // ── JANUAR ──────────────────────────────────────────────────────────
    add(1,  7,  "Рођење Господа Исуса Христа — Божић",           "Božić",             true);
    add(1, 13,  "Канун Богојављења — Badnji dan po starom",       "Badnji dan",        false);
    add(1, 14,  "Обрезање Господње / Свети Василије Велики",      "Sv. Vasilije",      true);
    add(1, 19,  "Богојављење — Крштење Господа Исуса Христа",     "Bogojavljenje",     true);
    add(1, 20,  "Сабор Светог Јована Крститеља",                  "Sv. Jovan",         false);
    add(1, 27,  "Свети Сава Српски — Просветитељ",                "Sveti Sava",        true);

    // ── FEBRUAR ─────────────────────────────────────────────────────────
    add(2, 14,  "Свети Трифун мученик",                            "Sv. Trifun",        false);
    add(2, 15,  "Сретење Господње",                                "Sretenje",          true);

    // ── MART ────────────────────────────────────────────────────────────
    add(3,  9,  "Свети Тарасије архиепископ",                      "Sv. Tarasije",      false);
    add(3, 22,  "Свети 40 Мученика Севастијских",                  "40 Mučenika",       false);

    // ── APRIL ───────────────────────────────────────────────────────────
    add(4,  7,  "Благовести — Васпоставитељска служба Пресветој Богородици", "Blagovesti", true);
    add(4, 23,  "Свети Великомученик Георгије — Ђурђевдан",        "Đurđevdan",         true);

    // ── MAJ ─────────────────────────────────────────────────────────────
    add(5,  6,  "Свети Јов Многострадалец",                        "Sv. Jov",           false);
    add(5, 21,  "Свети Цар Константин и царица Јелена",            "Sv. Konstantin",    true);

    // ── JUN ─────────────────────────────────────────────────────────────
    add(6,  3,  "Свети Алипије Столпник",                          "Sv. Alipije",       false);
    add(6, 24,  "Рођење Светог Јована Крститеља",                  "Rodj. Sv. Jovana",  true);
    add(6, 28,  "Видовдан — Свети кнез Лазар",                     "Vidovdan",          true);
    add(6, 29,  "Свети апостоли Петар и Павле — Петровдан",        "Petrovdan",         true);

    // ── JUL ─────────────────────────────────────────────────────────────
    add(7, 12,  "Прва и Друга налажење главе Светог Јована",       "Nalaženje glave",   false);
    add(7, 20,  "Света равноапостолна Марија Магдалина",            "Sv. Magdalina",     false);

    // ── AVGUST ──────────────────────────────────────────────────────────
    add(8,  2,  "Свети пророк Илија — Илиндан",                    "Ilindan",           true);
    add(8, 19,  "Преображење Господа Исуса Христа",                 "Preobraženje",      true);
    add(8, 28,  "Успење Пресвете Богородице — Велика Госпојина",   "Vel. Gospojina",    true);

    // ── SEPTEMBAR ───────────────────────────────────────────────────────
    add(9, 11,  "Усековање главе Светог Јована Крститеља",         "Usekovanje",        true);
    add(9, 14,  "Воздвижење Часног Крста",                          "Vozdviženje",       true);
    add(9, 21,  "Рођење Пресвете Богородице — Мала Госпојина",     "Mala Gospojina",    true);
    add(9, 27,  "Свети Апостол и јеванђелист Јован Богослов",      "Sv. Jovan Bogosl.", false);

    // ── OKTOBAR ─────────────────────────────────────────────────────────
    add(10, 14, "Покров Пресвете Богородице",                       "Pokrov",            true);

    // ── NOVEMBAR ────────────────────────────────────────────────────────
    add(11,  8, "Сабор Светих Арханђела — Аранђеловдан",           "Aranđelovdan",      true);
    add(11, 21, "Ваведење Пресвете Богородице у храм",              "Vavedenje",         true);

    // ── DECEMBAR ────────────────────────────────────────────────────────
    add(12,  9, "Зачеће Пресвете Богородице — Света Ана",           "Sv. Ana",           false);
    add(12, 19, "Свети Никола Чудотворац — Николиндан",             "Nikolindan",        true);
    add(12, 24, "Бадње Вече",                                        "Badnje veče",       true);
    add(12, 25, "Свети Спиридон Тримитунтски",                      "Sv. Spiridon",      false);

    // ── POKRETNI PRAZNICI (based on Orthodox Easter) ──────────────────
    QDate easter = calculateOrthodoxEaster(year);

    QDate palmSunday   = easter.addDays(-7);
    QDate holyFriday   = easter.addDays(-2);
    QDate holySaturday = easter.addDays(-1);
    QDate ascension    = easter.addDays(39);
    QDate pentecost    = easter.addDays(49);
    QDate allSaints    = easter.addDays(56);
    QDate apostlesFast = easter.addDays(57); // Start of Apostle's Fast

    map.insert(palmSunday,   {"Цвети — Улазак Господа у Јерусалим", "Cveti",          true});
    map.insert(holyFriday,   {"Велики петак — Распеће Господа",      "Vel. Petak",     true});
    map.insert(holySaturday, {"Велика субота",                         "Vel. Subota",    false});
    map.insert(easter,       {"Васкрсење Господа Исуса Христа — Ускрс", "Uskrs",       true});
    map.insert(ascension,    {"Вазнесење Господа Исуса Христа — Спасовдан", "Spasovdan", true});
    map.insert(pentecost,    {"Силазак Светог Духа — Духови / Тројице", "Duhovi",      true});
    map.insert(allSaints,    {"Недеља Свих Светих",                    "Sv. Sveti",    false});
    Q_UNUSED(apostlesFast);

    return map;
}

bool OrthodoxHolidays::isHoliday(const QDate& date, int year) {
    return getHolidaysForYear(year).contains(date);
}

bool OrthodoxHolidays::isGreatFeast(const QDate& date, int year) {
    auto map = getHolidaysForYear(year);
    if (map.contains(date))
        return map[date].isGreatFeast;
    return false;
}

HolidayInfo OrthodoxHolidays::getHolidayInfo(const QDate& date, int year) {
    auto map = getHolidaysForYear(year);
    if (map.contains(date))
        return map[date];
    return {"", "", false};
}

QList<QPair<QDate,QDate>> OrthodoxHolidays::getFastingPeriods(int year) {
    QList<QPair<QDate,QDate>> fasts;
    QDate easter = calculateOrthodoxEaster(year);

    // Veliki post (Great Lent): 48 days before Easter
    fasts.append({easter.addDays(-48), easter.addDays(-1)});
    // Apostolski post: Monday after All Saints to June 28 (Petrovdan eve)
    fasts.append({easter.addDays(57), QDate(year, 6, 28)});
    // Uspenjski post: August 1-14
    fasts.append({QDate(year, 8, 1), QDate(year, 8, 13)});
    // Božićni post: November 15 - January 6 (next year)
    fasts.append({QDate(year, 11, 15), QDate(year, 12, 31)});

    return fasts;
}

bool OrthodoxHolidays::isFastingDay(const QDate& date, int year) {
    // Wednesday and Friday are always fasting days (outside fast-free weeks)
    if (date.dayOfWeek() == 3 || date.dayOfWeek() == 5) {
        return true;
    }
    for (const auto& period : getFastingPeriods(year)) {
        if (date >= period.first && date <= period.second)
            return true;
    }
    return false;
}

#include "eventmanager.h"
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

#include "musicplayer.h"
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
#include <QStandardPaths>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QTime>
#include <QSizePolicy>

// ──────────────────────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────────────────────
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
}

MusicPlayer::~MusicPlayer() {}

// ── UI ────────────────────────────────────────────────────────────
void MusicPlayer::setupUI() {
    setMinimumHeight(130);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 8, 16, 8);
    mainLayout->setSpacing(6);

    // Track info row
    auto *infoRow = new QHBoxLayout();
    m_lblTitle  = new QLabel("♪  Nema pjesme", this);
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

    // Progress bar row
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

    m_btnShuffle  = makeBtn("⇄",  "Slučajni raspored");
    m_btnShuffle->setCheckable(true);
    m_btnRepeat   = makeBtn("↻",  "Ponavljaj");
    m_btnRepeat->setCheckable(true);
    m_btnPrev     = makeBtn("⏮",  "Prethodna");
    m_btnPlay     = makeBtn("▶",  "Reprodukuj / Pauza");
    m_btnPlay->setStyleSheet(QString(BTN_STYLE) +
        "QPushButton { font-size:22px; color:#e94560; }");
    m_btnNext     = makeBtn("⏭",  "Sljedeća");
    m_btnOpen     = makeBtn("📂", "Dodaj muziku");
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

    // Playlist panel (collapsible)
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
    btnRemove->setStyleSheet("QPushButton{background:rgba(233,69,96,0.15);color:#e94560;"
        "border:1px solid #e94560;border-radius:4px;padding:2px 8px;font-size:11px;}"
        "QPushButton:hover{background:rgba(233,69,96,0.3);}");
    auto *btnClear = new QPushButton("Očisti sve", m_playlistFrame);
    btnClear->setStyleSheet("QPushButton{background:rgba(255,255,255,0.05);color:#888;"
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
    connect(m_btnShuffle, &QPushButton::toggled, this, [this](bool c){ m_shuffle = c; });
    connect(m_btnRepeat,  &QPushButton::toggled, this, [this](bool c){ m_repeat  = c; });
    connect(m_btnPlaylistToggle, &QPushButton::toggled,
            m_playlistFrame, &QFrame::setVisible);
    connect(m_playlistWidget, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*){ onPlaylistDoubleClicked(m_playlistWidget->currentRow()); });
    connect(btnRemove, &QPushButton::clicked, this, &MusicPlayer::removeSelectedTrack);
    connect(btnClear,  &QPushButton::clicked, this, &MusicPlayer::clearPlaylist);
}

// ── Playback ──────────────────────────────────────────────────────
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
    // Highlight in playlist
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
    int next;
    if (m_shuffle) {
        next = QRandomGenerator::global()->bounded(m_playlist.size());
    } else {
        next = (m_currentIndex + 1) % m_playlist.size();
    }
    playIndex(next);
}

void MusicPlayer::prevTrack() {
    if (m_playlist.isEmpty()) return;
    int prev = (m_currentIndex - 1 + m_playlist.size()) % m_playlist.size();
    playIndex(prev);
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
}

void MusicPlayer::seekTo(int value) {
    m_player->setPosition((qint64)value);
}

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
    qint64 dur = m_player->duration();
    m_lblTime->setText(formatMs(pos) + " / " + formatMs(dur));
}

void MusicPlayer::onDurationChanged(qint64 dur) {
    m_sliderProgress->setRange(0, (int)dur);
}

void MusicPlayer::onPlaybackStateChanged(int /*state*/) {
    updatePlayButton();
}

void MusicPlayer::onMediaStatusChanged(int status) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (status == (int)QMediaPlayer::EndOfMedia) {
#else
    if (status == QMediaPlayer::EndOfMedia) {
#endif
        if (m_repeat) {
            m_player->setPosition(0);
            m_player->play();
        } else {
            nextTrack();
        }
    }
}

void MusicPlayer::onPlaylistDoubleClicked(int row) {
    playIndex(row);
}

void MusicPlayer::removeSelectedTrack() {
    int row = m_playlistWidget->currentRow();
    if (row < 0 || row >= m_playlist.size()) return;
    m_playlist.removeAt(row);
    delete m_playlistWidget->takeItem(row);
    if (m_currentIndex == row) {
        m_currentIndex = -1;
        m_player->stop();
        updateTrackLabel();
    } else if (m_currentIndex > row) {
        m_currentIndex--;
    }
}

void MusicPlayer::clearPlaylist() {
    m_playlist.clear();
    m_playlistWidget->clear();
    m_currentIndex = -1;
    m_player->stop();
    updateTrackLabel();
}

// ── Helpers ───────────────────────────────────────────────────────
void MusicPlayer::updatePlayButton() {
    bool playing = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    playing = (m_player->playbackState() == QMediaPlayer::PlayingState);
#else
    playing = (m_player->state() == QMediaPlayer::PlayingState);
#endif
    m_btnPlay->setText(playing ? "⏸" : "▶");
}

void MusicPlayer::updateTrackLabel() {
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size()) {
        m_lblTitle->setText("♪  Nema pjesme");
        m_lblArtist->setText("");
    } else {
        QString title = extractTitle(m_playlist[m_currentIndex]);
        m_lblTitle->setText("♪  " + title);
        m_lblArtist->setText(QString("Pjesma %1 od %2").arg(m_currentIndex+1).arg(m_playlist.size()));
    }
}

QString MusicPlayer::formatMs(qint64 ms) const {
    if (ms <= 0) return "0:00";
    int secs  = (int)(ms / 1000);
    int mins  = secs / 60;
    secs     %= 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}

QString MusicPlayer::extractTitle(const QUrl& url) const {
    QFileInfo fi(url.toLocalFile());
    return fi.completeBaseName();
}

bool MusicPlayer::isPlaying() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return m_player->playbackState() == QMediaPlayer::PlayingState;
#else
    return m_player->state() == QMediaPlayer::PlayingState;
#endif
}

QString MusicPlayer::currentTrackTitle() const {
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size())
        return "";
    return extractTitle(m_playlist[m_currentIndex]);
}

void MusicPlayer::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Glassmorphism panel
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1,1,-1,-1), 16, 16);
    p.fillPath(path, QColor(12, 12, 35, 200));
    p.setPen(QPen(QColor(255,255,255,18), 1));
    p.drawPath(path);
    // Subtle gradient top
    QLinearGradient grad(0,0,0,30);
    grad.setColorAt(0, QColor(83,52,131,40));
    grad.setColorAt(1, Qt::transparent);
    p.fillPath(path, grad);
}

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

#include <QApplication>
#include <QFont>
#include "mainwindow.h"

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
