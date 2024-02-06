#include "licenses_agreement.h"


LicensesAgreement::LicensesAgreement(QWidget *parent) :
        QDialog(parent), ui(new Ui::LicensesAgreement) {
    ui->setupUi(this);
}

LicensesAgreement::~LicensesAgreement() {
    delete ui;
}
