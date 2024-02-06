#include "widget_center.h"

WidgetCenter::WidgetCenter(QWidget *parent) :
        QWidget(parent), ui_(new Ui::WidgetCenter) {
    ui_->setupUi(this);

    this->initSelfDefinedUi();
    this->initTableView();
    this->initTreeView();
    this->initConnections();
}


WidgetCenter::~WidgetCenter() {
    delete ui_;
}

void WidgetCenter::initSelfDefinedUi() {
    ui_->splitter_2->setMouseTracking(true);
    ui_->splitter_2->setStretchFactor(0, 3);
    ui_->splitter_2->setStretchFactor(1, 6);
    ui_->splitter_2->setStretchFactor(2, 5);

    ui_->splitter->setMouseTracking(true);
    ui_->splitter->setStretchFactor(0, 6);
    ui_->splitter->setStretchFactor(1, 4);

    cur_path_ = QDir::currentPath();
    ui_->lineEdit->setText(cur_path_);
}

void WidgetCenter::initTreeView() {
    treeModel_file_ = new QFileSystemModel();
    treeModel_file_->setRootPath(cur_path_);
    ui_->treeView_file_system->setModel(treeModel_file_);
    ui_->treeView_file_system->setRootIndex(treeModel_file_->index(cur_path_));
    ui_->treeView_file_system->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->treeView_file_system->setColumnHidden(1, true);
    ui_->treeView_file_system->setColumnHidden(2, true);
    ui_->treeView_file_system->setColumnHidden(3, true);
    ui_->treeView_file_system->setStyleSheet("border: none");
    ui_->treeView_file_system->header()->setFrameShape(QFrame::NoFrame);
    ui_->treeView_file_system->header()->setFocusPolicy(Qt::NoFocus);
    ui_->treeView_file_system->header()->setStyleSheet("border: 0.5px solid rgb(140, 140, 140)");
}

void WidgetCenter::initTableView() {
    tabModel_var_space_ = new QStandardItemModel(30, 4, this);
    tabModel_var_space_->setHeaderData(0, Qt::Horizontal, tr("Name"));
    tabModel_var_space_->setHeaderData(1, Qt::Horizontal, tr("Value"));
    tabModel_var_space_->setHeaderData(2, Qt::Horizontal, tr("Type"));
    tabModel_var_space_->setHeaderData(3, Qt::Horizontal, tr("Size"));

    ui_->tableView_var_space->setModel(tabModel_var_space_);
    ui_->tableView_var_space->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableView_var_space->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableView_var_space->setFocusPolicy(Qt::NoFocus);
    ui_->tableView_var_space->verticalHeader()->setVisible(false);
    ui_->tableView_var_space->setFrameShape(QFrame::NoFrame);
    ui_->tableView_var_space->setStyleSheet("border: 0.5px solid rgb(140, 140, 140)");
    ui_->tableView_var_space->horizontalHeader()->setStyleSheet("border: none");
    ui_->tableView_var_space->horizontalHeader()->setHighlightSections(false);

    // ui_->tableView_var_space->setAlternatingRowColors(true);
    // QPalette palette;
    // palette.setColor(QPalette::Base, QColor());
    // palette.setColor(QPalette::AlternateBase, QColor("#5888d0"));
    // ui_->tableView_var_space->setPalette(palette);

    tabModel_fn_space_ = new QStandardItemModel(30, 3, this);
    tabModel_fn_space_->setHeaderData(0, Qt::Horizontal, tr("Name"));
    tabModel_fn_space_->setHeaderData(1, Qt::Horizontal, tr("Inputs"));
    tabModel_fn_space_->setHeaderData(2, Qt::Horizontal, tr("Outputs"));

    ui_->tableView_fn_space->setModel(tabModel_fn_space_);
    ui_->tableView_fn_space->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableView_fn_space->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableView_fn_space->setFocusPolicy(Qt::NoFocus);
    ui_->tableView_fn_space->verticalHeader()->setVisible(false);
    ui_->tableView_fn_space->setFrameShape(QFrame::NoFrame);
    ui_->tableView_fn_space->setStyleSheet("border: 0.5px solid rgb(140, 140, 140)");
    ui_->tableView_fn_space->horizontalHeader()->setStyleSheet("border: none");
    ui_->tableView_fn_space->horizontalHeader()->setHighlightSections(false);
}

void WidgetCenter::initConnections() {
    connect(ui_->tabWidget_workspace, SIGNAL(tabBarClicked(int)), this, SLOT(clickTabBar(int)));
    connect(this, SIGNAL(sig_createNewTab()), ui_->wgt_terminal, SLOT(createNewTab()));
    connect(ui_->wgt_terminal, SIGNAL(sig_changeCursorPos(int, int)), this, SLOT(getCursor(int, int)));
}

void WidgetCenter::clickTabBar(int id) {
    /*
    if (ui_->tabWidget_workspace->isTabVisible(id)){
        ui_->tabWidget_workspace->setTabVisible(id, false);
    } else{
        ui_->tabWidget_workspace->setTabVisible(id, true);
    }
    */
}

// this function is called by QAction act_new_kernel in mainwindow.cpp
void WidgetCenter::triggerNewTab() {
    emit sig_createNewTab();
}

void WidgetCenter::stopChannels() {
    // ui_->widget is an instance of EmbedJupyter
    ui_->wgt_terminal->stopChannels();
}

void WidgetCenter::shutDownKernels() {
    // ui_->widget is an instance of EmbedJupyter
    ui_->wgt_terminal->shutDownKernels();
}

void WidgetCenter::getCursor(int id_row, int id_col) {
    emit sig_changeCursor(id_row, id_col);
    // qDebug() << "widget-Center: " << id_row << id_col;
}