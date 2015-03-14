#ifndef EDITRULESDIALOG_H
#define EDITRULESDIALOG_H

#include <QDialog>

#include "rules.h"



namespace Ui {
class EditRulesDialog;
}

class EditRulesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditRulesDialog(Rules *rules, bool withApplyButton, QWidget *parent = 0);
    ~EditRulesDialog();

private:
    void setupUiFromRules();

    Ui::EditRulesDialog *ui;

    Rules               *mRules;
    bool                 mWasApplied;

private slots:
    void on_manuallyRadioButton_toggled(bool checked);
    void on_eachMinutesRadioButton_toggled(bool checked);
    void on_minutesSpinBox_valueChanged(int value);
    void on_daysRadioButton_toggled(bool checked);
    void on_mondayCheckBox_toggled(bool checked);
    void on_tuesdayCheckBox_toggled(bool checked);
    void on_wednesdayCheckBox_toggled(bool checked);
    void on_thursdayCheckBox_toggled(bool checked);
    void on_fridayCheckBox_toggled(bool checked);
    void on_saturdayCheckBox_toggled(bool checked);
    void on_sundayCheckBox_toggled(bool checked);
    void on_checkAutorunCheckBox_toggled(bool checked);
    void on_checkSystemFilesCheckBox_toggled(bool checked);
    void on_okButton_clicked();
    void on_applyButton_clicked();
    void on_cancelButton_clicked();
};

#endif // EDITRULESDIALOG_H
