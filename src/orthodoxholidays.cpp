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
