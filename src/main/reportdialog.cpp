#include "reportdialog.h"
#include "ui_reportdialog.h"

#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QDebug>



ReportDialog::ReportDialog(Rules *rules, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReportDialog)
    , mRules(rules)
{
    ui->setupUi(this);

    setWindowFlags(
                    Qt::Dialog
                    | Qt::CustomizeWindowHint
                    | Qt::WindowTitleHint
                    | Qt::WindowSystemMenuHint
                    | Qt::WindowMaximizeButtonHint
                    | Qt::WindowCloseButtonHint
                  );

    setupUiFromRules();
    loadWindowState();

    qDebug() << "Reports displayed";
}

ReportDialog::~ReportDialog()
{
    saveWindowState();

    delete ui;
}

void ReportDialog::setupUiFromRules()
{
    mSplitter = new QSplitter(Qt::Horizontal, this);

    ui->splitterLayout->removeWidget(ui->reportsTreeWidget);
    ui->splitterLayout->removeWidget(ui->beforeWidget);
    ui->splitterLayout->removeWidget(ui->afterWidget);

    mSplitter->addWidget(ui->reportsTreeWidget);
    mSplitter->addWidget(ui->beforeWidget);
    mSplitter->addWidget(ui->afterWidget);

    ui->splitterLayout->addWidget(mSplitter);

    // ---------------------------------------------------------------------------------

    QString reportsPath = QApplication::applicationDirPath() + "/" + mRules->getName() + "/Reports";
    QDir reportsDir(reportsPath);

    if (reportsDir.exists())
    {
        ui->stackedWidget->setCurrentWidget(ui->reportsPage);

        QTreeWidgetItem *lastItem = 0;

        QStringList reports = reportsDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

        for (int i = 0; i < reports.length(); ++i)
        {
            lastItem = 0;

            QTreeWidgetItem *reportItem = new QTreeWidgetItem(ui->reportsTreeWidget);
            reportItem->setText(0, QDateTime::fromString(reports.at(i), "yyyy_MM_dd_hh_mm_ss_zzz").toString("yyyy-MM-dd hh:mm:ss"));

            QDir reportDir(reportsPath + "/" + reports.at(i));
            QStringList categories = reportDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

            for (int j = 0; j < categories.length(); ++j)
            {
                QTreeWidgetItem *categoryItem = new QTreeWidgetItem(reportItem);
                categoryItem->setText(0, categories.at(j));

                QDir categoryDir(reportsPath + "/" + reports.at(i) + "/" + categories.at(j) + "/1_before");
                QStringList files = categoryDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

                for (int k = 0; k < files.length(); ++k)
                {
                    QTreeWidgetItem *fileItem = new QTreeWidgetItem(categoryItem);
                    fileItem->setText(0, files.at(k));
                    fileItem->setData(0, Qt::UserRole, reportsPath + "/" + reports.at(i) + "/" + categories.at(j) + "/" + files.at(k));

                    if (lastItem == 0)
                    {
                        lastItem = fileItem;
                    }
                }
            }
        }

        ui->reportsTreeWidget->expandAll();
        ui->reportsTreeWidget->setCurrentItem(lastItem);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->noReportsPage);
    }
}

void ReportDialog::on_reportsTreeWidget_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->reportsTreeWidget->currentItem();

    if (item)
    {
        QString filePath = item->data(0, Qt::UserRole).toString();

        int index = filePath.lastIndexOf("/");

        if (index >= 0)
        {
            QString beforeFilePath = filePath;
            QString afterFilePath  = filePath;

            beforeFilePath.insert(index, "/1_before");
            afterFilePath.insert(index,  "/2_after");

            QFile beforeFile(beforeFilePath);
            beforeFile.open(QIODevice::ReadOnly);
            ui->beforeTextEdit->setPlainText(QString::fromLocal8Bit(beforeFile.readAll()));
            beforeFile.close();

            QFile afterFile(afterFilePath);
            afterFile.open(QIODevice::ReadOnly);
            ui->afterTextEdit->setPlainText(QString::fromLocal8Bit(afterFile.readAll()));
            afterFile.close();
        }
        else
        {
            ui->beforeTextEdit->clear();
            ui->afterTextEdit->clear();
        }
    }
    else
    {
        ui->beforeTextEdit->clear();
        ui->afterTextEdit->clear();
    }
}

void ReportDialog::on_reportsTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    qDebug() << "Report double clicked";

    static QString mergeApp = "";

    if (mergeApp == "")
    {
#ifdef Q_OS_WIN
        {
            QStringList priorityList;
            priorityList.append("Araxis Merge");
            priorityList.append("WinMerge");

            QStringList appNames;
            appNames.append("Merge");
            appNames.append("WinMergeU");

            int priority = priorityList.length();



            QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);
            QStringList groups = settings.childGroups();

            for (int i = 0; i < groups.length(); ++i)
            {
                QString displayName = settings.value(groups.at(i) + "/DisplayName").toString();

                if (displayName != "")
                {
                    QString installLocation = settings.value(groups.at(i) + "/InstallLocation").toString();

                    if (installLocation != "")
                    {
                        for (int j = 0; j < priority; ++j)
                        {
                            if (installLocation.contains(priorityList.at(j), Qt::CaseInsensitive))
                            {
                                mergeApp = installLocation + "/" + appNames.at(j);
                                priority = j;

                                break;
                            }
                        }

                        if (priority == 0)
                        {
                            break;
                        }
                    }
                }
            }

            if (mergeApp == "")
            {
                if (QFile::exists("C:/Program Files/Araxis/Araxis Merge/Merge.exe"))
                {
                    mergeApp = "C:/Program Files/Araxis/Araxis Merge/Merge.exe";
                }
                else
                if (QFile::exists("C:/Program Files (x86)/Araxis/Araxis Merge/Merge.exe"))
                {
                    mergeApp = "C:/Program Files (x86)/Araxis/Araxis Merge/Merge.exe";
                }
                else
                if (QFile::exists("C:/Program Files/WinMerge/WinMergeU.exe"))
                {
                    mergeApp = "C:/Program Files/WinMerge/WinMergeU.exe";
                }
                else
                if (QFile::exists("C:/Program Files (x86)/WinMerge/WinMergeU.exe"))
                {
                    mergeApp = "C:/Program Files (x86)/WinMerge/WinMergeU.exe";
                }
            }

            mergeApp = QDir::fromNativeSeparators(mergeApp).replace("//", "/");
        }
#endif
    }

    qDebug() << "Merge application path:" << mergeApp;

    if (mergeApp != "")
    {
        QString filePath = item->data(0, Qt::UserRole).toString();

        int index = filePath.lastIndexOf("/");

        if (index >= 0)
        {
            QString beforeFilePath = filePath;
            QString afterFilePath  = filePath;

            beforeFilePath.insert(index, "/1_before");
            afterFilePath.insert(index,  "/2_after");



            QStringList arguments;
            arguments.append(beforeFilePath);
            arguments.append(afterFilePath);

            QProcess::startDetached(mergeApp, arguments);
        }
    }
    else
    {
#ifdef Q_OS_WIN
        {
            QMessageBox::information(this, tr("No Merge tool"), tr("Please install one of supported merge tools:\n    - Araxis Merge\n    - WinMerge"));
        }
#endif
    }
}

void ReportDialog::saveWindowState()
{
    QList<int> sizes = mSplitter->sizes();

    QSettings settings("GrisCom", "SystemAnalyzer");
    settings.setValue("ReportDialog/geometry", saveGeometry());

    for (int i = 0; i < sizes.length(); ++i)
    {
        settings.setValue("ReportDialog/splitter_" + QString::number(i + 1), sizes.at(i));
    }
}

void ReportDialog::loadWindowState()
{
    QList<int> sizes;

    QSettings settings("GrisCom", "SystemAnalyzer");
    restoreGeometry(settings.value("ReportDialog/geometry").toByteArray());

    for (int i = 0; i < mSplitter->count(); ++i)
    {
        bool ok;
        int size = settings.value("ReportDialog/splitter_" + QString::number(i + 1)).toInt(&ok);

        if (ok)
        {
            sizes.append(size);
        }
    }

    if (sizes.length() > 0)
    {
        mSplitter->setSizes(sizes);
    }
}
