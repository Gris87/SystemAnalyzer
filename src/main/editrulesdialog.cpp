#include "editrulesdialog.h"
#include "ui_editrulesdialog.h"



EditRulesDialog::EditRulesDialog(Rules *rules, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditRulesDialog)
    , mRules(rules)
{
    ui->setupUi(this);

    setupUiFromRules();
}

EditRulesDialog::~EditRulesDialog()
{
    delete ui;
}

void EditRulesDialog::setupUiFromRules()
{
    switch (mRules->getType())
    {
        case Rules::SCHEDULER_TYPE_MANUALLY:
        {
            ui->manuallyRadioButton->setChecked(true);
        }
        break;

        case Rules::SCHEDULER_TYPE_EACH_MINUTES:
        {
            ui->eachMinutesRadioButton->setChecked(true);

            ui->minutesSpinBox->setValue(mRules->getEachMinutes());
        }
        break;

        case Rules::SCHEDULER_TYPE_DAYS:
        {
            ui->daysRadioButton->setChecked(true);

            ui->mondayCheckBox->setChecked(   mRules->isMondayEnabled());
            ui->tuesdayCheckBox->setChecked(  mRules->isTuesdayEnabled());
            ui->wednesdayCheckBox->setChecked(mRules->isWednesdayEnabled());
            ui->thursdayCheckBox->setChecked( mRules->isThursdayEnabled());
            ui->fridayCheckBox->setChecked(   mRules->isFridayEnabled());
            ui->saturdayCheckBox->setChecked( mRules->isSaturdayEnabled());
            ui->sundayCheckBox->setChecked(   mRules->isSundayEnabled());
        }
        break;

        default:
            qFatal("Unknown scheduler type");
        break;
    }

    ui->checkAutorunCheckBox->setChecked(    mRules->isCheckAutorun());
    ui->checkSystemFilesCheckBox->setChecked(mRules->isCheckSystemFiles());

    ui->applyButton->setEnabled(false);
}

void EditRulesDialog::on_manuallyRadioButton_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_eachMinutesRadioButton_toggled(bool /*checked*/)
{
    ui->minutesSpinBox->setEnabled(checked);
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_minutesSpinBox_valueChanged(int /*value*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_daysRadioButton_toggled(bool /*checked*/)
{
    ui->daysWidget->setEnabled(checked);
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_mondayCeckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_tuesdayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_wednesdayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_thursdayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_fridayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_saturdayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_sundayCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_checkAutorunCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_checkSystemFilesCheckBox_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_okButton_clicked()
{
    if (ui->applyButton->isEnabled())
    {
        on_applyButton_clicked();
    }

    accept();
}

void EditRulesDialog::on_applyButton_clicked()
{
    if (ui->manuallyRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULER_TYPE_MANUALLY);
    }
    else
    if (ui->eachMinutesRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULER_TYPE_EACH_MINUTES);

        mRules->setEachMinutes(ui->minutesSpinBox->value());
    }
    else
    if (ui->daysRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULER_TYPE_DAYS);

        mRules->setMondayEnabled(   ui->mondayCheckBox->isChecked());
        mRules->setTuesdayEnabled(  ui->tuesdayCheckBox->isChecked());
        mRules->setWednesdayEnabled(ui->wednesdayCheckBox->isChecked());
        mRules->setThursdayEnabled( ui->thursdayCheckBox->isChecked());
        mRules->setFridayEnabled(   ui->fridayCheckBox->isChecked());
        mRules->setSaturdayEnabled( ui->saturdayCheckBox->isChecked());
        mRules->setSundayEnabled(   ui->sundayCheckBox->isChecked());
    }
    else
    {
        mRules->setType(Rules::SCHEDULER_TYPE_MANUALLY);
    }

    mRules->setCheckAutorun(    ui->checkAutorunCheckBox->isChecked());
    mRules->setCheckSystemFiles(ui->checkSystemFilesCheckBox->isChecked());
}

void EditRulesDialog::on_cancelButton_clicked()
{
    reject();
}
