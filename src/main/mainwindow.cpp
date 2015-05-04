#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProgressBar>
#include <QMessageBox>

#include "aboutdialog.h"
#include "editrulesdialog.h"
#include "trayicon.h"



#define RULES_TABLE_COLUMN_SCHEDULE 0
#define RULES_TABLE_COLUMN_PROGRESS 1



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mAllowClose(false)
{
    ui->setupUi(this);

    if (!TrayIcon::isSystemTrayAvailable())
    {
        mAllowClose = true;
    }
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

void MainWindow::updateRulesRow(int row)
{
    ui->rulesTableWidget->item(row, RULES_TABLE_COLUMN_SCHEDULE)->setText(mRulesList.at(row)->toString());
}

void MainWindow::addRulesRow()
{
    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setMaximum(100);
    progressBar->setTextVisible(false);

    // ------------------------------------------------

    int lastRow = ui->rulesTableWidget->rowCount();

    mRulesList.at(lastRow)->setProgressBar(progressBar);

    ui->rulesTableWidget->setRowCount(lastRow + 1);
    ui->rulesTableWidget->setItem(      lastRow, RULES_TABLE_COLUMN_SCHEDULE, new QTableWidgetItem());
    ui->rulesTableWidget->setCellWidget(lastRow, RULES_TABLE_COLUMN_PROGRESS, progressBar);

    updateRulesRow(lastRow);
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
    Rules *newRules = new Rules(this);

    EditRulesDialog dialog(newRules, false, this);

    if (dialog.exec())
    {
        mRulesList.append(newRules);
        addRulesRow();

        newRules->checkIfNeedStart();

        ui->rulesTableWidget->setCurrentCell(mRulesList.length() - 1, 0);
    }
    else
    {
        delete newRules;
    }
}

void MainWindow::on_actionEdit_triggered()
{
    int row = ui->rulesTableWidget->currentRow();

    EditRulesDialog dialog(mRulesList.at(row), true, this);

    if (dialog.exec())
    {
        updateRulesRow(row);

        mRulesList.at(row)->checkIfNeedStart();
    }
}

void MainWindow::on_actionRemove_triggered()
{
    if (QMessageBox::question(this, tr("Remove rules"), tr("Do you want to remove selected rules?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes)
    {
        QSet<int> rowsSet;

        QList<QTableWidgetSelectionRange> ranges = ui->rulesTableWidget->selectedRanges();

        for (int i = 0; i < ranges.length(); ++i)
        {
            const QTableWidgetSelectionRange &range = ranges.at(i);

            for (int j = range.topRow(); j <= range.bottomRow(); ++j)
            {
                rowsSet.insert(j);
            }
        }

        QList<int> rowsList = rowsSet.toList();

        if (rowsList.length() > 0)
        {
            qSort(rowsList);

            for (int i = rowsList.length() - 1; i >= 0; --i)
            {
                delete mRulesList.at(rowsList.at(i));
                mRulesList.removeAt(rowsList.at(i));

                ui->rulesTableWidget->removeRow(rowsList.at(i));
            }

            ui->rulesTableWidget->setCurrentCell(rowsList.first() < ui->rulesTableWidget->rowCount() ? rowsList.first() : ui->rulesTableWidget->rowCount() - 1, 0);
        }
    }
}

void MainWindow::on_actionStart_triggered()
{
    // TODO: Implement MainWindow::on_actionStart_triggered
}

void MainWindow::on_rulesTableWidget_currentCellChanged(int currentRow, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
    if (currentRow >= 0)
    {
        ui->actionEdit->setEnabled(  true);
        ui->actionRemove->setEnabled(true);
        ui->actionStart->setEnabled( true);
    }
    else
    {
        ui->actionEdit->setEnabled(  false);
        ui->actionRemove->setEnabled(false);
        ui->actionStart->setEnabled( false);
    }
}

void MainWindow::on_rulesTableWidget_cellDoubleClicked(int /*row*/, int /*column*/)
{
    on_actionEdit_triggered();
}
