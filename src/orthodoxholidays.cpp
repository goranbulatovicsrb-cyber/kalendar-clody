#include "orthodoxholidays.h"
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
