#ifndef SCLAB_DONATION_OR_SPONSOR_H
#define SCLAB_DONATION_OR_SPONSOR_H

#include <QDialog>
#include "ui_donation_or_sponsor.h"


QT_BEGIN_NAMESPACE
namespace Ui { class DonationOrSponsor; }
QT_END_NAMESPACE

class DonationOrSponsor : public QDialog {
Q_OBJECT

public:
    explicit DonationOrSponsor(QWidget *parent = nullptr);

    ~DonationOrSponsor() override;

private:
    Ui::DonationOrSponsor *ui;
};


#endif //SCLAB_DONATION_OR_SPONSOR_H
