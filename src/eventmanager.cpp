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
