#pragma once
#include <QWidget>
#include <QDate>
#include <QMap>
#include "orthodoxholidays.h"
#include "eventmanager.h"

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
    void  drawDayCell(QPainter& p, const QRectF& r, const QDate& date,
                      bool isCurrentMonth, bool isToday,
                      bool isSelected, bool isHovered,
                      const HolidayInfo& holiday, bool hasEvents) const;
    void  rebuildHolidays();

    EventManager *m_events;
    QDate  m_currentDate;
    QDate  m_selectedDate;
    QDate  m_hoveredDate;
    QDate  m_today;

    QMap<QDate, HolidayInfo> m_holidays;

    float  m_headerH = 42.0f;
    float  m_dayH    = 28.0f;
    float  cellH     = 72.0f;
    int    m_gridRows = 6;

    static const QString kDayNames[7];
};
