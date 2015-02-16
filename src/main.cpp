#include <QApplication>

#include "main/mainwindow.h"
#include "main/trayicon.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    if (TrayIcon::isSystemTrayAvailable())
    {
        global::trayIcon.init(&w);
        global::trayIcon.show();
    }
    else
    {
        w.show();
    }

    return a.exec();
}
