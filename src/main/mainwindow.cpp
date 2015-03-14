#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "editrulesdialog.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mAllowClose(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < mRulesList.length(); ++i)
    {
        delete mRulesList.at(i);
    }

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!mAllowClose)
    {
        event->ignore();
        hide();
    }
}

void MainWindow::closeWindow()
{
    mAllowClose = true;

    close();
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
    raise();
    activateWindow();
}

void MainWindow::trayIconExitClicked()
{
    closeWindow();
}

void MainWindow::on_actionExit_triggered()
{
    closeWindow();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionAdd_triggered()
{
    Rules *newRules = new Rules();

    EditRulesDialog dialog(newRules, false, this);

    if (dialog.exec())
    {
        mRulesList.append(newRules);
    }
    else
    {
        delete newRules;
    }
}

void MainWindow::on_actionEdit_triggered()
{
    // TODO: Implement MainWindow::on_actionEdit_triggered
}

void MainWindow::on_actionRemove_triggered()
{
    // TODO: Implement MainWindow::on_actionRemove_triggered
}

void MainWindow::on_actionStart_triggered()
{
    // TODO: Implement MainWindow::on_actionStart_triggered
}
