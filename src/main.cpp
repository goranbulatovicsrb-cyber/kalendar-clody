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
