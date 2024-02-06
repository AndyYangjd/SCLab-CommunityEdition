#include "kernel_selection.h"

KernelSelection::KernelSelection(QWidget *parent) :
        QDialog(parent), ui_(new Ui::KernelSelection) {
    ui_->setupUi(this);

    connect(ui_->buttonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(getSelectedKernelName(QAbstractButton *)));
    connect(ui_->btn_confirm, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui_->btn_cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

KernelSelection::~KernelSelection() {
    delete ui_;
}

void KernelSelection::getSelectedKernelName(QAbstractButton * btn) {
    QString btn_name = btn->objectName();
    // qDebug() << QString("Clicked Button: %1").arg(btn_name);

    // QList<QAbstractButton *> list = ui_->buttonGroup->buttons();
    // foreach(QAbstractButton * p_btn, list){
    //         QString strStatus = p_btn->isChecked() ? "Checked" : "Unchecked";
    //         qDebug() << QString("Button : %1 is %2").arg(btn_name).arg(strStatus);
    // }
    if (btn_name == "rBtn_jl"){
        re_kernel_name_ = "julia";
    }else if (btn_name == "rBtn_py"){
        re_kernel_name_ = "python";
    }else if (btn_name == "rBtn_home"){
        re_kernel_name_="home";
    } else if (btn_name == "rBtn_blend"){
        re_kernel_name_="blend";
    } else{
        re_kernel_name_ = "";
    }
}