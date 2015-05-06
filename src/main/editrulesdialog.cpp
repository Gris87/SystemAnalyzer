#include "editrulesdialog.h"
#include "ui_editrulesdialog.h"

#include <QMessageBox>
#include <QDebug>



EditRulesDialog::EditRulesDialog(Rules *rules, bool withApplyButton, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditRulesDialog)
    , mRules(rules)
    , mWasApplied(false)
{
    ui->setupUi(this);

    setWindowFlags(
                    Qt::Dialog
                    | Qt::CustomizeWindowHint
                    | Qt::WindowTitleHint
                    | Qt::WindowSystemMenuHint
                    | Qt::WindowCloseButtonHint
                  );

    ui->applyButton->setVisible(withApplyButton);

    setupUiFromRules();

    qDebug() << "Rules editor displayed";
}

EditRulesDialog::~EditRulesDialog()
{
    delete ui;
}

void EditRulesDialog::setupUiFromRules()
{
    switch (mRules->getType())
    {
        case Rules::SCHEDULE_TYPE_MANUALLY:
        {
            ui->manuallyRadioButton->setChecked(true);
        }
        break;

        case Rules::SCHEDULE_TYPE_EACH_MINUTES:
        {
            ui->eachMinutesRadioButton->setChecked(true);

            ui->minutesSpinBox->setValue(mRules->getEachMinutes());
        }
        break;

        case Rules::SCHEDULE_TYPE_DAYS:
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
            qFatal("Unknown schedule type");
        break;
    }

    ui->checkAutorunCheckBox->setChecked(    mRules->isCheckAutorun());
    ui->checkSystemFilesCheckBox->setChecked(mRules->isCheckSystemFiles());

    // TODO: Verification of paths

    ui->applyButton->setEnabled(false);
}

bool EditRulesDialog::apply()
{
    if (mRules->isRunning())
    {
        if (QMessageBox::question(this, tr("Terminate"), tr("Rules are running now. You have to terminate it. Do you want to continue?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes)
        {
            mRules->stop();
            mRules->waitForFinished();
        }
        else
        {
            return false;
        }
    }

    mWasApplied = true;

    mRules->reset();

    if (ui->manuallyRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULE_TYPE_MANUALLY);
    }
    else
    if (ui->eachMinutesRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULE_TYPE_EACH_MINUTES);

        mRules->setEachMinutes(ui->minutesSpinBox->value());
    }
    else
    if (ui->daysRadioButton->isChecked())
    {
        mRules->setType(Rules::SCHEDULE_TYPE_DAYS);

        mRules->setMondayEnabled(   ui->mondayCheckBox->isChecked());
        mRules->setTuesdayEnabled(  ui->tuesdayCheckBox->isChecked());
        mRules->setWednesdayEnabled(ui->wednesdayCheckBox->isChecked());
        mRules->setThursdayEnabled( ui->thursdayCheckBox->isChecked());
        mRules->setFridayEnabled(   ui->fridayCheckBox->isChecked());
        mRules->setSaturdayEnabled( ui->saturdayCheckBox->isChecked());
        mRules->setSundayEnabled(   ui->sundayCheckBox->isChecked());

        if ((mRules->getDaysMask() & Rules::SCHEDULE_DAYS_MASK) == 0)
        {
            mRules->setType(Rules::SCHEDULE_TYPE_MANUALLY);
        }
    }
    else
    {
        qFatal("Unknown schedule type");
    }

    mRules->setCheckAutorun(    ui->checkAutorunCheckBox->isChecked());
    mRules->setCheckSystemFiles(ui->checkSystemFilesCheckBox->isChecked());

    // TODO: Verification of paths

    mRules->save();

    qDebug() << "Rules changes applied";

    return true;
}

void EditRulesDialog::on_manuallyRadioButton_toggled(bool /*checked*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_eachMinutesRadioButton_toggled(bool checked)
{
    ui->minutesSpinBox->setEnabled(checked);
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_minutesSpinBox_valueChanged(int /*value*/)
{
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_daysRadioButton_toggled(bool checked)
{
    ui->daysWidget->setEnabled(checked);
    ui->applyButton->setEnabled(true);
}

void EditRulesDialog::on_mondayCheckBox_toggled(bool /*checked*/)
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
        if (!apply())
        {
            return;
        }
    }

    accept();

    qDebug() << "Rules editor closed";
}

void EditRulesDialog::on_applyButton_clicked()
{
    if (apply())
    {
        ui->applyButton->setEnabled(false);
    }
}

void EditRulesDialog::on_cancelButton_clicked()
{
    if (mWasApplied)
    {
        accept();
    }
    else
    {
        reject();
    }

    qDebug() << "Rules editor closed";
}
