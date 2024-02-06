#include "donation_or_sponsor.h"

DonationOrSponsor::DonationOrSponsor(QWidget *parent) :
        QDialog(parent), ui(new Ui::DonationOrSponsor) {
    ui->setupUi(this);
}

DonationOrSponsor::~DonationOrSponsor() {
    delete ui;
}
