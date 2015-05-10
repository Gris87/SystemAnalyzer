#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

#include <QTreeWidgetItem>
#include <QSplitter>

#include "rules.h"



namespace Ui {
class ReportDialog;
}



class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(Rules *rules, QWidget *parent = 0);
    ~ReportDialog();

private:
    void setupUiFromRules();
    void saveWindowState();
    void loadWindowState();

    Ui::ReportDialog    *ui;
    QSplitter           *mSplitter;

    Rules               *mRules;

private slots:
    void on_reportsTreeWidget_itemSelectionChanged();
    void on_reportsTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
};

#endif // REPORTDIALOG_H
