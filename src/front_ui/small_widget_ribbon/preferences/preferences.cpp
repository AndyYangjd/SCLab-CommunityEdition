#include "preferences.h"

Preferences::Preferences(QWidget *parent) :
        QDialog(parent), ui_(new Ui::Preferences) {
    ui_->setupUi(this);

    setSelfDefinedUI();
    setConnections();
}

Preferences::~Preferences() {
    delete ui_;
}

void Preferences::setSelfDefinedUI() {
    auto* act_search = new QAction(ui_->lineEdit_search);
    act_search->setIcon(QIcon(":/image/image/search.png"));
    ui_->lineEdit_search->addAction(act_search, QLineEdit::LeadingPosition);
}

void Preferences::setConnections() {
    connect(ui_->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onTreeWidgetItemClicked(QTreeWidgetItem *, int)));
}

void Preferences::onTreeWidgetItemClicked(QTreeWidgetItem * item, int col) {
    auto str_item =  item->text(col);

    if (str_item=="Appearance"){
        ui_->stackedWidget->setCurrentWidget(ui_->page_appearance);
    }else if (str_item=="Keymap"){
        ui_->stackedWidget->setCurrentWidget(ui_->page_keymap);
    }else if (str_item=="System Settings") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_system_setting);
    }else if (str_item=="Editor") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_general);
    }else if (str_item=="General") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_general);
    }else if (str_item=="Font") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_font);
    }else if (str_item=="Set Paths") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_set_paths);
    }else if (str_item=="Plugins") {
        ui_->stackedWidget->setCurrentWidget(ui_->page_plugins);
    }else{
        ;
    }
}
