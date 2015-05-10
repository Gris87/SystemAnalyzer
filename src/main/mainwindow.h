#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>
#include <QCloseEvent>

#include "rules.h"



namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    void closeWindow();
    void updateRulesRow(int row);
    void addRulesRow();

    void saveWindowState();
    void loadWindowState();
    void loadRules();

    Ui::MainWindow *ui;

    bool            mAllowClose;
    QList<Rules *>  mRulesList;
    QIcon           mPauseIcon;
    QIcon           mStartIcon;

public slots:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void trayIconShowClicked();
    void trayIconExitClicked();

private slots:
    void rulesStarted();
    void rulesFinished(bool isReportCreated);

    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionAdd_triggered();
    void on_actionEdit_triggered();
    void on_actionRemove_triggered();
    void on_actionStart_triggered();
    void on_actionReports_triggered();
    void on_rulesTableWidget_itemSelectionChanged();
    void on_rulesTableWidget_cellDoubleClicked(int row, int column);
};

#endif // MAINWINDOW_H
