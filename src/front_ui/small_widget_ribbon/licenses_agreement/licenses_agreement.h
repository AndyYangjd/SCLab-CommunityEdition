#ifndef SCLAB_LICENSES_AGREEMENT_H
#define SCLAB_LICENSES_AGREEMENT_H

#include <QDialog>
#include "ui_licenses_agreement.h"


QT_BEGIN_NAMESPACE
namespace Ui { class LicensesAgreement; }
QT_END_NAMESPACE

class LicensesAgreement : public QDialog {
Q_OBJECT

public:
    explicit LicensesAgreement(QWidget *parent = nullptr);

    ~LicensesAgreement() override;

private:
    Ui::LicensesAgreement *ui;
};


#endif //SCLAB_LICENSES_AGREEMENT_H
