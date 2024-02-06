#include "welcome_home.h"


WelcomeHome::WelcomeHome(QWidget *parent) :
        QWidget(parent), ui(new Ui::WelcomeHome) {
    ui->setupUi(this);
}

WelcomeHome::~WelcomeHome() {
    delete ui;
}
