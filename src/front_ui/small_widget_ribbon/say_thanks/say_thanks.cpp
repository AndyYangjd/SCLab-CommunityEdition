#include "say_thanks.h"


SayThanks::SayThanks(QWidget *parent) :
        QDialog(parent), ui(new Ui::SayThanks) {
    ui->setupUi(this);
}

SayThanks::~SayThanks() {
    delete ui;
}
