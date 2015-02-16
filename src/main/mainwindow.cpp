#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
        {
            trayIconShowClicked();
        }
        break;

        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
        {
            // Nothing
        }
        break;

        default:
        {
            qFatal("Unknown activation reason %u", reason);
        }
        break;
    }
}

void MainWindow::trayIconShowClicked()
{
    show();
}

void MainWindow::trayIconExitClicked()
{
    QApplication::closeAllWindows();
}
