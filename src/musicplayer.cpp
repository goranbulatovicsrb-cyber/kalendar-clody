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
    m_lblTitle->setStyleSheet("color:#ffffff; font-size:13px; font-weight:700; background:transparent;");
    m_lblTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_lblTitle->setMinimumWidth(100);
    m_lblArtist = new QLabel("", this);
    m_lblArtist->setStyleSheet("color:#aaaacc; font-size:11px; background:transparent;");
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
        "QFrame { background: #0d0d2b; border-radius:8px; border:1px solid rgba(153,102,204,0.4); }");
    auto *plLayout = new QVBoxLayout(m_playlistFrame);
    plLayout->setContentsMargins(8, 6, 8, 6);
    plLayout->setSpacing(4);
    auto *plHeader = new QHBoxLayout();
    auto *plTitle = new QLabel("🎵  Plejlista", m_playlistFrame);
    plTitle->setStyleSheet("color:#ccccee; font-size:12px; font-weight:700; background:transparent;");
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
    m_playlistWidget->setMinimumHeight(180);
    m_playlistWidget->setMaximumHeight(320);
    m_playlistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_playlistWidget->setStyleSheet(
        "QListWidget { background:#070718; border:1px solid rgba(153,102,204,0.35); border-radius:6px; color:#e8e8ff; font-size:12px; outline:none; }"
        "QListWidget::item { padding:6px 10px; color:#e8e8ff; border-bottom:1px solid rgba(255,255,255,0.04); }"
        "QListWidget::item:hover { background:rgba(153,102,204,0.25); color:#ffffff; }"
        "QListWidget::item:selected { background:rgba(233,69,96,0.35); color:#ffffff; font-weight:600; }"
        "QScrollBar:vertical { background:#0a0a20; width:8px; border-radius:4px; }"
        "QScrollBar::handle:vertical { background:rgba(153,102,204,0.6); border-radius:4px; min-height:20px; }"
        "QScrollBar::handle:vertical:hover { background:rgba(153,102,204,0.9); }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0px; }");
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
