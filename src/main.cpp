#include "ui/WindowMain/mainwindow.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("GrzybDev");
    QCoreApplication::setOrganizationDomain("grzyb.dev");
    QCoreApplication::setApplicationName("OpenMic Server");

    QTranslator appTranslator;
    QTranslator qtTranslator;

    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &localeCode : uiLanguages) {
        const QLocale locale = QLocale(localeCode);

        const QString localeName = locale.name();
        const QString localeShortName = localeName.split('_').first();
        const QString languageName = locale.languageToString(locale.language());

        const QString appLocalePath = ":/i18n/OpenMicServer_" + localeName;
        const QString qtLocalePath = "qt_" + localeShortName;

        if (appTranslator.load(appLocalePath) && qtTranslator.load(qtLocalePath, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&appTranslator);
            app.installTranslator(&qtTranslator);

            qDebug() << "Successfully set application language to" << languageName;
            break;
        }

        qDebug() << "Failed to set application language to" << languageName << "(" << localeName << "), either application or Qt doesn't support your language!";
    }

    MainWindow win;
    return app.exec();
}
