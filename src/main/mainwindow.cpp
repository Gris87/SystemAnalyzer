#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QDebug>

#include "aboutdialog.h"
#include "editrulesdialog.h"
#include "reportdialog.h"
#include "trayicon.h"



#define RULES_TABLE_COLUMN_SCHEDULE 0
#define RULES_TABLE_COLUMN_PROGRESS 1



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mAllowClose(false)
    , mPauseIcon(":/images/Pause.png")
    , mStartIcon(":/images/Start.png")
{
    ui->setupUi(this);

    ui->rulesTableWidget->installEventFilter(this);

    if (!TrayIcon::isSystemTrayAvailable())
    {
        mAllowClose = true;
    }

    loadWindowState();
    loadRules();
}

MainWindow::~MainWindow()
{
    saveWindowState();

    for (int i = 0; i < mRulesList.length(); ++i)
    {
        Rules *rules = mRulesList.at(i);

        if (rules->isRunning())
        {
            rules->stop();
        }
    }

    for (int i = 0; i < mRulesList.length(); ++i)
    {
        Rules *rules = mRulesList.at(i);

        if (rules->isRunning())
        {
            rules->waitForFinished();
        }

        delete rules;
    }

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mAllowClose)
    {
        qDebug() << "Application closed";
    }
    else
    {
        event->ignore();
        hide();

        qDebug() << "Main window moved to tray";
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->rulesTableWidget)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if (keyEvent->key() == Qt::Key_Delete)
            {
                if (ui->actionRemove->isEnabled())
                {
                    on_actionRemove_triggered();
                }

                return true;
            }
        }
    }

    return QObject::eventFilter(object, event);
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
    Rules *rules = mRulesList.at(lastRow);

    rules->setProgressBar(progressBar);

    connect(rules, SIGNAL(started()),      this, SLOT(rulesStarted()));
    connect(rules, SIGNAL(finished(bool)), this, SLOT(rulesFinished(bool)));

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
            qDebug() << "Tray icon double clicked";

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

    qDebug() << "Main window displayed";
}

void MainWindow::trayIconExitClicked()
{
    closeWindow();
}

void MainWindow::rulesStarted()
{
    qDebug() << "On rules started";

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

        bool atleastOneStarted = false;

        for (int i = 0; i < rowsList.length(); ++i)
        {
            if (mRulesList.at(rowsList.at(i))->isRunning())
            {
                atleastOneStarted = true;
                break;
            }
        }

        if (atleastOneStarted)
        {
            ui->actionStart->setIcon(mPauseIcon);
        }
        else
        {
            ui->actionStart->setIcon(mStartIcon);
        }
    }
}

void MainWindow::rulesFinished(bool isReportCreated)
{
    qDebug() << "On rules finished";

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

        bool atleastOneStarted = false;

        for (int i = 0; i < rowsList.length(); ++i)
        {
            Rules *rules = mRulesList.at(rowsList.at(i));

            if (rules->isRunning() && rules != sender())
            {
                atleastOneStarted = true;
                break;
            }
        }

        if (atleastOneStarted)
        {
            ui->actionStart->setIcon(mPauseIcon);
        }
        else
        {
            ui->actionStart->setIcon(mStartIcon);
        }
    }

    if (isReportCreated)
    {
        if (!isVisible())
        {
            trayIconShowClicked();
        }

        ReportDialog dialog(reinterpret_cast<Rules *>(sender()), this);
        dialog.exec();
    }
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
    newRules->setName(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz"));

    EditRulesDialog dialog(newRules, false, this);

    if (dialog.exec())
    {
        mRulesList.append(newRules);
        addRulesRow();

        newRules->checkIfNeedStart();

        ui->rulesTableWidget->setCurrentCell(mRulesList.length() - 1, 0);

        qDebug() << "New rules added";
    }
    else
    {
        delete newRules;
    }
}

void MainWindow::on_actionEdit_triggered()
{
    int row = ui->rulesTableWidget->currentRow();
    Rules *rules = mRulesList.at(row);

    EditRulesDialog dialog(rules, true, this);

    if (dialog.exec())
    {
        updateRulesRow(row);

        rules->checkIfNeedStart();
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

            for (int i = 0; i < rowsList.length(); ++i)
            {
                Rules *rules = mRulesList.at(rowsList.at(i));

                if (rules->isRunning())
                {
                    rules->stop();
                }
            }

            for (int i = 0; i < rowsList.length(); ++i)
            {
                Rules *rules = mRulesList.at(rowsList.at(i));

                if (rules->isRunning())
                {
                    rules->waitForFinished();
                }
            }

            for (int i = rowsList.length() - 1; i >= 0; --i)
            {
                Rules *rules = mRulesList.at(rowsList.at(i));

                rules->deleteFolder();
                delete rules;
                mRulesList.removeAt(rowsList.at(i));

                ui->rulesTableWidget->removeRow(rowsList.at(i));
            }

            ui->rulesTableWidget->setCurrentCell(rowsList.first() < ui->rulesTableWidget->rowCount() ? rowsList.first() : ui->rulesTableWidget->rowCount() - 1, 0);

            qDebug() << "Rules removed";
        }
    }
}

void MainWindow::on_actionStart_triggered()
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

        bool atleastOneStarted = false;

        for (int i = 0; i < rowsList.length(); ++i)
        {
            if (mRulesList.at(rowsList.at(i))->isRunning())
            {
                atleastOneStarted = true;
                break;
            }
        }

        if (atleastOneStarted)
        {
            for (int i = 0; i < rowsList.length(); ++i)
            {
                if (mRulesList.at(rowsList.at(i))->isRunning())
                {
                    mRulesList.at(rowsList.at(i))->stop();
                }
            }

            for (int i = 0; i < rowsList.length(); ++i)
            {
                if (mRulesList.at(rowsList.at(i))->isRunning())
                {
                    mRulesList.at(rowsList.at(i))->waitForFinished();
                }
            }

            ui->actionStart->setIcon(mStartIcon);

            qDebug() << "All rules stopped";
        }
        else
        {
            for (int i = 0; i < rowsList.length(); ++i)
            {
                if (!mRulesList.at(rowsList.at(i))->isRunning())
                {
                    mRulesList.at(rowsList.at(i))->start();
                }
            }

            ui->actionStart->setIcon(mPauseIcon);

            qDebug() << "All rules started";
        }
    }
}

void MainWindow::on_actionReports_triggered()
{
    ReportDialog dialog(mRulesList.at(ui->rulesTableWidget->currentRow()), this);
    dialog.exec();
}

void MainWindow::on_rulesTableWidget_itemSelectionChanged()
{
    qDebug() << "Rules selection changed";

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

        bool atleastOneStarted = false;

        for (int i = 0; i < rowsList.length(); ++i)
        {
            if (mRulesList.at(rowsList.at(i))->isRunning())
            {
                atleastOneStarted = true;
                break;
            }
        }

        if (atleastOneStarted)
        {
            ui->actionStart->setIcon(mPauseIcon);
        }
        else
        {
            ui->actionStart->setIcon(mStartIcon);
        }

        ui->actionEdit->setEnabled(   true);
        ui->actionRemove->setEnabled( true);
        ui->actionStart->setEnabled(  true);
        ui->actionReports->setEnabled(true);
    }
    else
    {
        ui->actionStart->setIcon(mStartIcon);

        ui->actionEdit->setEnabled(   false);
        ui->actionRemove->setEnabled( false);
        ui->actionStart->setEnabled(  false);
        ui->actionReports->setEnabled(false);
    }
}

void MainWindow::on_rulesTableWidget_cellDoubleClicked(int /*row*/, int /*column*/)
{
    qDebug() << "Rules double clicked";

    on_actionReports_triggered();
}

void MainWindow::saveWindowState()
{
    QSettings settings("GrisCom", "SystemAnalyzer");
    settings.setValue("MainWindow/geometry",    saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
}

void MainWindow::loadWindowState()
{
    QSettings settings("GrisCom", "SystemAnalyzer");
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
}

void MainWindow::loadRules()
{
    QString dir = QApplication::applicationDirPath();

    QDir appDir(dir);
    QStringList folders = appDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < folders.length(); ++i)
    {
        if (QFile::exists(dir + "/" + folders.at(i) + "/config.ini"))
        {
            Rules *newRules = new Rules(this);

            newRules->setName(folders.at(i));
            newRules->load();

            mRulesList.append(newRules);
        }
    }

    if (mRulesList.length() == 0)
    {
        Rules *newRules;

        // ---------------------------------------------------------

        newRules = new Rules(this);

        newRules->setName("default_1");
        newRules->setType(Rules::SCHEDULE_TYPE_EACH_MINUTES);
        newRules->setEachMinutes(5);
        newRules->setCheckAutorun(true);
        newRules->setCheckSystemFiles(false);
        newRules->save();

        mRulesList.append(newRules);

        // ---------------------------------------------------------

        newRules = new Rules(this);

        newRules->setName("default_2");
        newRules->setType(Rules::SCHEDULE_TYPE_EACH_MINUTES);
        newRules->setEachMinutes(15);
        newRules->setCheckAutorun(false);
        newRules->setCheckSystemFiles(true);
        newRules->save();

        mRulesList.append(newRules);

        // ---------------------------------------------------------

        qDebug() << "Initial rules created";
    }

    for (int i = 0; i < mRulesList.length(); ++i)
    {
        addRulesRow();
        mRulesList.at(i)->checkIfNeedStart();
    }

    ui->rulesTableWidget->setCurrentCell(0, 0);
}
