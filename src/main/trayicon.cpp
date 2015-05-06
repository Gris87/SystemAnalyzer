#include <QMenu>
#include <QDebug>

#include "trayicon.h"



namespace global
{

TrayIcon trayIcon;

}

// =================================================================

TrayIcon::TrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
{
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::init(MainWindow *mainWindow)
{
    QMenu *trayIconMenu = new QMenu(mainWindow);

    QAction *defaultAction = trayIconMenu->addAction(tr("Show"), mainWindow, SLOT(trayIconShowClicked()));
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(tr("Exit"), mainWindow, SLOT(trayIconExitClicked()));

    trayIconMenu->setDefaultAction(defaultAction);

    setIcon(QIcon(":/images/Icon.png"));
    setContextMenu(trayIconMenu);
    setToolTip(tr("System Analyzer"));

    QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), mainWindow, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));

    qDebug() << "Tray icon initialized";
}
