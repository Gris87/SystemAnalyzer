#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>

#include "mainwindow.h"



class TrayIcon : public QSystemTrayIcon
{
public:
    explicit TrayIcon(QObject *parent = 0);
    ~TrayIcon();

    void init(MainWindow *mainWindow);
};

// =================================================================

namespace global
{
extern TrayIcon trayIcon;
}

#endif // TRAYICON_H
