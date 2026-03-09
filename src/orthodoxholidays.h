#pragma once
#include <QDate>
#include <QMap>
#include <QString>
#include <QList>

struct HolidayInfo {
    QString name;        // Full name
    QString shortName;   // Short display name
    bool isGreatFeast;   // Great feast = red letters
};

class OrthodoxHolidays {
public:
    // Calculate Orthodox Easter (Gregorian date)
    static QDate calculateOrthodoxEaster(int year);

    // Get all holidays for a given year
    static QMap<QDate, HolidayInfo> getHolidaysForYear(int year);

    // Check if date is a holiday
    static bool isHoliday(const QDate& date, int year);
    static bool isGreatFeast(const QDate& date, int year);

    // Get holiday info
    static HolidayInfo getHolidayInfo(const QDate& date, int year);

    // Fasting periods (start, end)
    static QList<QPair<QDate,QDate>> getFastingPeriods(int year);
    static bool isFastingDay(const QDate& date, int year);
};
