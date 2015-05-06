#include <QApplication>
#include <QDebug>

#include "main/mainwindow.h"
#include "main/trayicon.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "Application started";

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
