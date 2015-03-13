#include "editrulesdialog.h"
#include "ui_editrulesdialog.h"



EditRulesDialog::EditRulesDialog(Rules *rules, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditRulesDialog)
    , mRules(rules)
{
    ui->setupUi(this);
}

EditRulesDialog::~EditRulesDialog()
{
    delete ui;
}
