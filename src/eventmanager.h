#pragma once
#include <QObject>
#include <QDate>
#include <QTime>
#include <QString>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QColor>

struct CalendarEvent {
    QString id;
    QDate   date;
    QTime   time;
    QString title;
    QString description;
    bool    hasReminder;
    int     reminderMinsBefore; // minutes before event
    QColor  color;
};

class QSystemTrayIcon;

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
