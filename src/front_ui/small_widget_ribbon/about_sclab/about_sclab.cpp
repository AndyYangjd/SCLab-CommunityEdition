#include "about_sclab.h"

AboutSClab::AboutSClab(QWidget *parent) :
        QDialog(parent), ui_(new Ui::AboutSClab) {
    ui_->setupUi(this);

    connect(ui_->btn_ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui_->btn_copy, SIGNAL(clicked()), this, SLOT(reject()));
}

AboutSClab::~AboutSClab() {
    delete ui_;
}

void AboutSClab::setStatement(const QString &str) {
    ui_->textEdit->setText(str);
}

QString AboutSClab::getStatement() {
    return ui_->textEdit->toPlainText();
}
