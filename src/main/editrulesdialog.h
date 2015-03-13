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
    explicit EditRulesDialog(Rules *rules, QWidget *parent = 0);
    ~EditRulesDialog();

private:
    Ui::EditRulesDialog *ui;

    Rules               *mRules;
};

#endif // EDITRULESDIALOG_H
