#pragma once
#include <QWidget>
#include <QUrl>
#include <QList>
#include <QTimer>
#include <QColor>
#include <QPixmap>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  #include <QMediaPlayer>
  #include <QAudioOutput>
#else
  #include <QMediaPlayer>
  #include <QMediaPlaylist>
#endif

class QSlider;
class QLabel;
class QPushButton;
class QListWidget;
class QFrame;

class MusicPlayer : public QWidget {
    Q_OBJECT
public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer() override;

    QString currentTrackTitle() const;
    bool isPlaying() const;

signals:
    void trackChanged(const QString& title);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void togglePlayPause();
    void nextTrack();
    void prevTrack();
    void openFiles();
    void toggleShuffle();
    void toggleRepeat();
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
    QPixmap makeIcon(const QString& type, QColor color, int size) const;

    QMediaPlayer *m_player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *m_audioOutput;
#endif
    QList<QUrl> m_playlist;
    int  m_currentIndex = -1;
    bool m_shuffle      = false;
    bool m_repeat       = false;
    bool m_expanding    = false;

    QPushButton *m_btnPlay;
    QPushButton *m_btnPrev;
    QPushButton *m_btnNext;
    QPushButton *m_btnShuffle;
    QPushButton *m_btnRepeat;
    QPushButton *m_btnOpen;
    QPushButton *m_btnPlaylistToggle;
    QSlider     *m_sliderProgress;
    QSlider     *m_sliderVolume;
    QLabel      *m_lblTitle;
    QLabel      *m_lblArtist;
    QLabel      *m_lblTime;
    QLabel      *m_lblVolIcon;
    QListWidget *m_playlistWidget;
    QFrame      *m_playlistFrame;
};
