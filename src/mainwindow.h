#pragma once
#include <QMainWindow>
#include <QDate>
#include <QSystemTrayIcon>
#include "eventmanager.h"

class CalendarWidget;
class EventManager;
class MusicPlayer;
class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QTimer;
class QFrame;

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

    // Core
    EventManager  *m_events;
    CalendarWidget *m_calendar;
    MusicPlayer    *m_player;
    QSystemTrayIcon *m_tray;
    QTimer         *m_clockTimer;
    QDate           m_displayDate;
    QDate           m_selectedDate;

    // Navigation bar
    QFrame     *m_navBar;
    QPushButton *m_btnPrev;
    QPushButton *m_btnNext;
    QPushButton *m_btnToday;
    QPushButton *m_btnFullscreen;
    QPushButton *m_btnAbout;
    QLabel     *m_lblMonth;
    QLabel     *m_lblClock;
    QLabel     *m_lblHoliday;

    // Side panel
    QFrame     *m_sidePanel;
    QLabel     *m_lblSelectedDate;
    QLabel     *m_lblHolidayInfo;
    QListWidget *m_eventList;
    QPushButton *m_btnAddEvent;
    QPushButton *m_btnEditEvent;
    QPushButton *m_btnDelEvent;
};
