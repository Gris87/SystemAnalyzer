#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>

#include "main/mainwindow.h"
#include "main/trayicon.h"



void makeAutorun()
{
#ifdef Q_OS_WIN
    {
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

        settings.setValue("SystemAnalyzer", QDir::toNativeSeparators(QApplication::applicationFilePath()));
    }
#endif

    qDebug() << "Application in autorun now";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "Application started";

    makeAutorun();

    MainWindow w;

    if (TrayIcon::isSystemTrayAvailable())
    {
        qDebug() << "Tray icon supported";

        global::trayIcon.init(&w);
        global::trayIcon.show();
    }
    else
    {
        qDebug() << "No tray icon";

        w.show();
    }

    return a.exec();
}
