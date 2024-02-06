#include "terminal_simulation.h"


TerminalSimulation::TerminalSimulation(QWidget *parent) :
        QWidget(parent), ui_(new Ui::TerminalSimulation) {
    ui_->setupUi(this);

    this->initSelfDefinedUi();
    this->initConnections();

    generateConnectionFile();
    createKernelManagerAndClient();
}

TerminalSimulation::~TerminalSimulation() {
    delete ui_;
    delete jl_kernel_manager_;
    delete py_kernel_manager_;

    for (const auto item: vec_jl_kernel_client_) {
        delete item;
    }
    for (const auto item: vec_py_kernel_client_) {
        delete item;
    }

}


void TerminalSimulation::initSelfDefinedUi() {
    toolBtn_ = new QToolButton(this);
    toolBtn_->setIcon(QIcon(":/image/image/newlybuild.png"));
    toolBtn_->setAutoRaise(true);
    ui_->tabWgt->setCornerWidget(toolBtn_, Qt::TopLeftCorner);
    createHomeWidget();
}

void TerminalSimulation::initConnections() {
    connect(ui_->tabWgt, SIGNAL(tabCloseRequested(int)), this, SLOT(tapClosed(int)));
    connect(toolBtn_, SIGNAL(clicked()), this, SLOT(createNewTab()));
}

void TerminalSimulation::createNewTab() {
    KernelSelection* dlg_kernel_select = new KernelSelection(this);
    int ret = dlg_kernel_select->exec();
    if (ret==QDialog::Accepted){
        new_kernel_name_=dlg_kernel_select->re_kernel_name_;
    } else{
        new_kernel_name_="";
    }
    delete dlg_kernel_select;

    if (new_kernel_name_!="") {
        auto tab_new = new JupyterWidget(this);
        tab_new->setStyleSheet("background-color:white;color: black");
        if (new_kernel_name_ == "julia") {
            tab_new->setObjectName(QString::fromUtf8("tapJulia"));

            QIcon icon;
            icon.addFile(QString::fromUtf8(":/image/image/julia.png"), QSize(), QIcon::Normal, QIcon::Off);
            ui_->tabWgt->addTab(tab_new, icon, QString());

            tab_new->setKernelName("julia");
            tab_new->setKernelManager(jl_kernel_manager_);
            tab_new->setKernelClient(vec_jl_kernel_client_.at(0));

            num_kernel_++;
        } else if (new_kernel_name_ == "python") {
            tab_new->setObjectName(QString::fromUtf8("tapPython"));

            QIcon icon;
            icon.addFile(QString::fromUtf8(":/image/image/python-logo"), QSize(), QIcon::Normal, QIcon::Off);
            ui_->tabWgt->addTab(tab_new, icon, QString());

            tab_new->setKernelName("python");
            tab_new->setKernelManager(py_kernel_manager_);
            tab_new->setKernelClient(vec_py_kernel_client_.at(0));

            num_kernel_++;
        }else if (new_kernel_name_ == "home"){
            QString str_first_tap_name = ui_->tabWgt->widget(0)->objectName();
            if (str_first_tap_name!="tapWelcomeHome"){
                createHomeWidget();
            }
        } else if (new_kernel_name_ == "blend"){
            QMessageBox box;
            box.setWindowTitle("Blend Kernel");
            box.setText("Blend kernel will bring out in the future.");
            box.exec();
        }else{
            ;
        }

        // initialize the edit
        tab_new->reset(true);

        // initialize the connections
        connect(tab_new, SIGNAL(changeTextCursor(int, int)), this, SLOT(getCursorPosition(int, int)));
    }

    ui_->tabWgt->setCurrentIndex(ui_->tabWgt->count()-1);

    // close the Tap-Home automatically
    QString str_first_tap_name = ui_->tabWgt->widget(0)->objectName();
    if ((ui_->tabWgt->count()>1) && str_first_tap_name=="tapWelcomeHome"){
        ui_->tabWgt->removeTab(0);
    }

    // append the kernel-client to the kernel-widget
}

void TerminalSimulation::tapClosed(int id_closed_tap) {
    QString closed_tap_object_name = ui_->tabWgt->widget(id_closed_tap)->objectName();
    if (closed_tap_object_name!="tapWelcomeHome"){
        num_kernel_--;
    }

    ui_->tabWgt->removeTab(id_closed_tap);

    if (ui_->tabWgt->count()==0)
    {
        createHomeWidget();
    }
}

void TerminalSimulation::createHomeWidget() {
    QWidget* tap_widget_home = new QWidget(this);
    tap_widget_home->setObjectName(QString::fromUtf8("tapWelcomeHome"));
    tap_widget_home->setStyleSheet("background-color:white;color: black");

    QVBoxLayout* vBox_home= new QVBoxLayout(tap_widget_home);
    vBox_home->setSpacing(0);
    vBox_home->setObjectName(QString::fromUtf8("vbox_home"));
    vBox_home->setContentsMargins(0, 0, 0, 0);

    QFrame* hLine_home_top = new QFrame(tap_widget_home);
    hLine_home_top->setObjectName(QString::fromUtf8("hLine_home_top"));
    hLine_home_top->setFrameShape(QFrame::HLine);
    hLine_home_top->setFrameShadow(QFrame::Sunken);
    vBox_home->addWidget(hLine_home_top);

    WelcomeHome* welcome_home = new WelcomeHome(tap_widget_home);
    welcome_home->setObjectName("welcome");
    vBox_home->addWidget(welcome_home);

    QFrame* hLine_home_below = new QFrame(tap_widget_home);
    hLine_home_below->setObjectName(QString::fromUtf8("hLine_home_below"));
    hLine_home_below->setFrameShape(QFrame::HLine);
    hLine_home_below->setFrameShadow(QFrame::Sunken);
    vBox_home->addWidget(hLine_home_below);

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/image/home.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui_->tabWgt->addTab(tap_widget_home, icon, QString());
}

void TerminalSimulation::createKernelManagerAndClient() {
    jl_kernel_manager_ = new KernelManager(this, "julia", connection_file_list_.at(0), true);
    jl_kernel_manager_->startKernel();
    KernelClient* jl_kernel_client = jl_kernel_manager_->getNewClient();
    jl_kernel_client->startChannels();
    vec_jl_kernel_client_.push_back(jl_kernel_client);

    py_kernel_manager_ = new KernelManager(this, "python3", connection_file_list_.at(3), true);
    py_kernel_manager_->startKernel();
    KernelClient* py_kernel_client = py_kernel_manager_->getNewClient();
    py_kernel_client->startChannels();
    vec_py_kernel_client_.push_back(py_kernel_client);
}

void TerminalSimulation::generateConnectionFile() {
    QString cur_file_path = QDir::currentPath();
    project_path_ = cur_file_path.mid(0, cur_file_path.lastIndexOf("/"));
    const QString kernel_path = project_path_.append("/").append("kernels");

    QString os_type = QSysInfo::productType();

    for (int i = 0; i < MAX_NUM_KERNELS; ++i) {
        QUuid uuid = QUuid::createUuid();
        QString str_uuid = uuid.toString().remove("{").remove("}");

        auto conn_file_path = QString("%1/kernel-%2.json").arg(kernel_path).arg(str_uuid);

        if (os_type=="windows"){
            conn_file_path.replace("/", "\\");
        }
        connection_file_list_.append(conn_file_path);
    }
}

// This function will be called after the click the X to quit the app
void TerminalSimulation::stopChannels() {

}

// This function will be called after the click the X to quit the app
void TerminalSimulation::shutDownKernels() {

}

void TerminalSimulation::getCursorPosition(int id_row, int id_col) {
    // qDebug() << "Row: " << id_row << " Col: " << id_col;
    emit sig_changeCursorPos(id_row, id_col);
}

/*
QWidget* TerminalSimulation::createKernelWidget(){
    QWidget* tap_widget = new QWidget(this);
    tap_widget->setAutoFillBackground(false);

    QVBoxLayout* vBox=new QVBoxLayout(tap_widget);
    vBox->setSpacing(5);
    vBox->setObjectName(QString::fromUtf8("verticalLayout"));
    vBox->setContentsMargins(0, 0, 0, 0);

    QFrame* h_line = new QFrame(tap_widget);
    h_line->setFrameShape(QFrame::HLine);
    vBox->addWidget(h_line);

    auto jupyter_widget = new JupyterWidget(tap_widget);
    vBox->addWidget(jupyter_widget);
    return tap_widget;

    auto jupyter_widget = new JupyterWidget(this);
    return jupyter_widget;
}
*/