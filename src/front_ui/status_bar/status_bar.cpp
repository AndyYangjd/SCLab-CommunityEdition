#include "status_bar.h"


StatusBar::StatusBar(QWidget *parent) :
        QWidget(parent), ui_(new Ui::StatusBar) {
    ui_->setupUi(this);
}

StatusBar::~StatusBar() {
    delete ui_;
}

void StatusBar::updateCursorPos(int id_row, int id_col) {
    // qDebug() << id_row << id_col << "StatusBar";
    if (id_row>=0 && id_col >=0){
        auto str = QString("Cursor: %1:%2").arg(id_row).arg(id_col);
        ui_->label_cursor->setText(str);
    } else{
        ui_->label_cursor->setText("Cursor: ---:--");
    }
}