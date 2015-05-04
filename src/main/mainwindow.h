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

private:
    void closeWindow();
    void updateRulesRow(int row);
    void addRulesRow();

    Ui::MainWindow *ui;

    bool            mAllowClose;
    QList<Rules *>  mRulesList;

public slots:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void trayIconShowClicked();
    void trayIconExitClicked();

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionAdd_triggered();
    void on_actionEdit_triggered();
    void on_actionRemove_triggered();
    void on_actionStart_triggered();
    void on_rulesTableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_rulesTableWidget_cellDoubleClicked(int row, int column);    
};

#endif // MAINWINDOW_H
