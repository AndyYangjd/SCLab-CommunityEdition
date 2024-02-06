#include <QApplication>
#include <QFile>
#include <QTextEdit>
#include <QAbstractButton>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QStatusBar>
#include <QDebug>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QFontComboBox>
#include <QLabel>

#include "SARibbon/SAFramelessHelper.h"
#include "SARibbon/SARibbonCustomizeDialog.h"
#include "SARibbon/SARibbonMenu.h"
#include "SARibbon/SARibbonComboBox.h"
#include "SARibbon/SARibbonLineEdit.h"
#include "SARibbon/SARibbonGallery.h"
#include "SARibbon/SARibbonCheckBox.h"
#include "SARibbon/SARibbonQuickAccessBar.h"
#include "SARibbon/SARibbonButtonGroupWidget.h"
#include "SARibbon/SARibbonApplicationButton.h"
#include "SARibbon/SARibbonCustomizeWidget.h"
#include "SARibbon/SARibbonElementManager.h"
#include "SARibbon/SARibbonBar.h"
#include "SARibbon/SARibbonCategory.h"
#include "SARibbon/SARibbonPannel.h"
#include "SARibbon/SARibbonToolButton.h"

#include "main_window.h"


MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par), m_customizeWidget(nullptr)
{
    SARibbonBar* ribbon = ribbonBar();
    // ribbon->applicationButton()->setText("SCLab");
    ribbon->applicationButton()->hide();

    // create the central widget
    widget_center_ = new WidgetCenter(this);
    setCentralWidget(widget_center_);

    //添加主标签页 - 通过addCategoryPage工厂函数添加
    SARibbonCategory* categoryHome = ribbon->addCategoryPage(tr("Home"));
    categoryHome->setObjectName(("categoryHome"));
    createCategoryHome(categoryHome);

    //添加其他标签页 - 直接new SARibbonCategory添加
    SARibbonCategory* categoryPkg = new SARibbonCategory();
    categoryPkg->setCategoryName(tr("Pkg-Manage"));
    categoryPkg->setObjectName(("categoryPkg"));
    createCategoryPkg(categoryPkg);
    ribbon->addCategoryPage(categoryPkg);

    SARibbonCategory* categoryPlots = new SARibbonCategory();
    categoryPlots->setCategoryName(("Plots"));
    categoryPlots->setObjectName(("categoryPlots"));
    ribbon->addCategoryPage(categoryPlots);
    createCategoryPlots(categoryPlots);

    SARibbonCategory* categoryApps = new SARibbonCategory();
    categoryApps->setCategoryName(("Apps"));
    categoryApps->setObjectName(("categoryApp"));
    ribbon->addCategoryPage(categoryApps);
    createCategoryApps(categoryApps);

    createContextCategory1();
    createContextCategory2();

    // create the Access-Bar
    SARibbonQuickAccessBar* quickAccessBar = ribbon->quickAccessBar();
    createQuickAccessBar(quickAccessBar);

    SARibbonButtonGroupWidget* rightBar = ribbon->rightButtonGroup();
    createRightButtonGroup(rightBar);

    // define other actions
    addSomeOtherAction();

    // set Status-Bar
    status_bar_ = new QStatusBar();
    auto* widget_status_bar = new StatusBar(status_bar_);
    // status_bar_->addWidget(widget_status_bar);
    status_bar_->addPermanentWidget(widget_status_bar, 10);
    connect(widget_center_, SIGNAL(sig_changeCursor(int, int)), widget_status_bar, SLOT(updateCursorPos(int, int)));

    setStatusBar(status_bar_);

    // define title and icon of this software
    setWindowIcon(QIcon(":/image/image/sc_v12.png"));
    setWindowTitle(sw_type_);


    // set the size of view
    setMinimumWidth(800);
    showMaximized();

    // set the style
    // ribbonBar()->setRibbonStyle(static_cast< SARibbonBar::RibbonStyle >(0));
}

void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast< SARibbonBar::RibbonStyle >(id));
}

void MainWindow::onActionCustomizeTriggered(bool b)
{
    Q_UNUSED(b);
    if (nullptr == m_customizeWidget) {
        m_customizeWidget = new SARibbonCustomizeWidget(this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
        m_customizeWidget->setWindowModality(Qt::ApplicationModal);  //设置阻塞类型
        m_customizeWidget->setAttribute(Qt::WA_ShowModal, true);     //属性设置 true:模态 false:非模态
        m_customizeWidget->setupActionsManager(m_actMgr);
    }
    m_customizeWidget->show();
    m_customizeWidget->applys();
}

void MainWindow::onActionCustomizeAndSaveTriggered(bool b)
{
    Q_UNUSED(b);
    SARibbonCustomizeDialog dlg(this);
    dlg.setupActionsManager(m_actMgr);
    dlg.fromXml("customize.xml");
    if (SARibbonCustomizeDialog::Accepted == dlg.exec()) {
        dlg.applys();
        QByteArray str;
        QXmlStreamWriter xml(&str);
        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
        xml.setCodec("utf-8");
#endif
        xml.writeStartDocument();
        bool isok = dlg.toXml(&xml);
        xml.writeEndDocument();
        if (isok) {
            QFile f("customize.xml");
            if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream s(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QTextStream always encodes XML in UTF-8.
                s.setCodec("utf-8");
#endif
                s << str;
                s.flush();
            }
            m_edit->append("write xml:");
            m_edit->append(str);
        }
    }
}

void MainWindow::onNotificationsTriggered()
{

}

void MainWindow::onActionRemoveAppBtnTriggered(bool b)
{
    if (b) {
        ribbonBar()->setApplicationButton(nullptr);
    } else {
        auto* actionRemoveAppBtn = new SARibbonApplicationButton();
        actionRemoveAppBtn->setText(tr("File"));
        this->ribbonBar()->setApplicationButton(actionRemoveAppBtn);
    }
}

void MainWindow::onActionUseQssTriggered()
{
    QFile f("ribbon.qss");
    if (!f.exists()) {
        QString fdir = QFileDialog::getOpenFileName(this, tr("select qss file"));
        if (fdir.isEmpty()) {
            return;
        }
        f.setFileName(fdir);
    }
    if (!f.open(QIODevice::ReadWrite)) {
        return;
    }
    QString qss(f.readAll());
    m_edit->setText(qss);
    this->ribbonBar()->setStyleSheet(qss);
}

void MainWindow::onActionLoadCustomizeXmlFileTriggered()
{
    //只能调用一次
    static bool has_call = false;
    if (!has_call) {
        has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
    }
}

void MainWindow::onActionWindowFlagNormalButtonTriggered(bool b)
{
    if (b) {
        //最大最小关闭按钮都有
        Qt::WindowFlags f = windowFlags();
        f |= (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        updateWindowFlag(f);
    } else {
        //由于已经处于frameless状态，这个最大最小设置是无效的
        // setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
        Qt::WindowFlags f = windowFlags();
        f &= ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowCloseButtonHint;
        updateWindowFlag(f);
    }
}

void MainWindow::onFontComWidgetCurrentFontChanged(const QFont& f)
{
    // qDebug() << "set font:" << f;
    ribbonBar()->setFont(f);
    update();
}

void MainWindow::onActionFontLargerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() + 1);
    ribbonBar()->setFont(f);
    update();
    // qDebug() << "set font:" << f;
}

void MainWindow::onActionFontSmallerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() - 1);
    ribbonBar()->setFont(f);
    update();
    // qDebug() << "set font:" << f;
}

void MainWindow::onActionwordWrapIn2rowTriggered(bool b)
{
    SARibbonToolButton::setLiteStyleEnableWordWrap(b);  //设置是否允许2行模式下文字换行，换行的话图标会较小
    //换行设定后需要重新计算样式尺寸
    RibbonSubElementStyleOpt.recalc();
    //通过setRibbonStyle来让ribbonbar重绘
    //由于关键尺寸变化了，需要重新布局
    ribbonBar()->updateRibbonGeometry();
}

void MainWindow::createCategoryHome(SARibbonCategory* page)
{
    //-------------- panel---file
    SARibbonPannel* panel_file = page->addPannel(("File"));

    // new script
    auto * menu_new_script = new SARibbonMenu(this);
    QAction* act_script_sclab = createAction("Local", ":/image/image/script.png");
    menu_new_script->addAction(act_script_sclab);
    menu_new_script->addSeparator();
    QAction* act_script_vscode = createAction("VS Code", ":/image/image/visual-studio-code.png");
    menu_new_script->addAction(act_script_vscode);
    QAction* act_script_notepad = createAction("Notepad++", ":/image/image/notepad.png");
    menu_new_script->addAction(act_script_notepad);
    QAction* act_script_subl = createAction("Sublime Text", ":/image/image/sublime-text.png");
    menu_new_script->addAction(act_script_subl);

    QAction* act_new_script= createAction(tr("new               Script"), ":/image/image/file-code-one.png");
    act_new_script->setShortcut(QKeySequence(QLatin1String("Ctrl+N")));
    act_new_script->setMenu(menu_new_script);
    auto btn_new_script =panel_file->addLargeAction(act_new_script);
    btn_new_script->setPopupMode(QToolButton::MenuButtonPopup);

    // new terminal
    QAction* act_new_terminal= createAction(tr("new Terminal"), ":/image/image/terminal.png");
    panel_file->addLargeAction(act_new_terminal);

    // new Kernel
    QAction* act_new_kernel= createAction(tr("new      Kernel"), ":/image/image/newlybuild_black.png");
    connect(act_new_kernel, SIGNAL(triggered()), widget_center_, SLOT(triggerNewTab()));
    panel_file->addLargeAction(act_new_kernel);

    // New
    auto* menu_new = new SARibbonMenu(this);
    QAction* act_new_project= createAction(tr("new Project"), ":/image/image/project.png");
    menu_new->addAction(act_new_project);
    menu_new->addSeparator();
    QAction* act_new_file_editing= createAction(tr("new File"), ":/image/image/file-editing-one.png");
    menu_new->addAction(act_new_file_editing);
    QAction* act_new_folder= createAction(tr("new Folder"), ":/image/image/folder-plus.png");
    menu_new->addAction(act_new_folder);
    menu_new->addSeparator();
    QAction* act_new_script2= createAction(tr("new Script"), ":/image/image/file-code-one.png");
    menu_new->addAction(act_new_script2);
    menu_new->addAction(act_new_terminal);
    QAction* act_new_kernel2= createAction(tr("new Kernel"), ":/image/image/newlybuild_black.png");
    menu_new->addAction(act_new_kernel2);
    connect(act_new_kernel2, SIGNAL(triggered()), widget_center_, SLOT(triggerNewTab()));

    QAction* act_new = createAction(tr("New"), ":/image/image/add.png");
    act_new->setMenu(menu_new);
    SARibbonToolButton* btn_new = panel_file->addLargeAction(act_new);
    btn_new->setPopupMode(QToolButton::MenuButtonPopup);

    // Open
    SARibbonMenu* menu_open = new SARibbonMenu(this);
    QAction* act_open_project = createAction(tr("open Project"), ":/image/image/project.png");
    menu_open->addAction(act_open_project);
    menu_open->addSeparator();
    QAction* act_open_script = createAction(tr("open Script"), ":/image/image/file-code-one.png");
    menu_open->addAction(act_open_script);
    QAction* act_open_file = createAction(tr("open File"), ":/image/image/file-text-one.png");
    menu_open->addAction(act_open_file);
    QAction* act_open_folder = createAction(tr("open Folder"), ":/image/image/folder-open.png");
    menu_open->addAction(act_open_folder);

    QAction* act_open = createAction(tr("Open"), ":/image/image/folder-open.png");
    act_open->setMenu(menu_open);
    SARibbonToolButton* btn_open = panel_file->addLargeAction(act_open);
    btn_open->setPopupMode(QToolButton::MenuButtonPopup);

    panel_file->addSeparator();

    // Find Files
    QAction* act_find_files = createAction(tr("Find Files"), ":/image/image/find.png");
    panel_file->addMediumAction(act_find_files);

    // Compare
    QAction* act_compare = createAction(tr("Compare"), ":/image/image/contrast-view.png");
    panel_file->addSmallAction(act_compare);


    // panel---Variable
    SARibbonMenu* menu = new SARibbonMenu(this);
    SARibbonToolButton* btn;

    SARibbonPannel* panel_workspace = page->addPannel(("Work Space"));

    QAction* act_import_data= createAction(tr("import Data"), ":/image/image/data-switching.png");
    act_import_data->setCheckable(true);
    panel_workspace->addLargeAction(act_import_data);

    QAction* act_save_workspace= createAction(tr("save WorkSapce"), ":/image/image/table-file.png");
    act_save_workspace->setCheckable(true);
    panel_workspace->addLargeAction(act_save_workspace);

    panel_workspace->addSeparator();

    QAction* act_new_variable = createAction(tr("new Variable"), ":/image/image/new-document.png");
    panel_workspace->addSmallAction(act_new_variable);

    QAction* act_open_variable = createAction(tr("open Variable"), ":/image/image/open-one.png");
    act_open_variable->setMenu(menu);
    act_open_variable->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    btn = panel_workspace->addSmallAction(act_open_variable);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    QAction* act_clear_workspace = createAction(tr("clear VarSpace"), ":/image/image/clear.png");
    act_clear_workspace->setMenu(menu);
    act_clear_workspace->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = panel_workspace->addSmallAction(act_clear_workspace);
    btn->setPopupMode(QToolButton::MenuButtonPopup);


    // panel---kernel
    SARibbonPannel* panel_kernel = page->addPannel(("Kernel"));

    SARibbonMenu* menu_magic = new SARibbonMenu(this);
    QAction* act_magic_clc= createAction(tr("clc"), "");
    menu_magic->addAction(act_magic_clc);
    QAction* act_magic_clear= createAction(tr("clear"), "");
    menu_magic->addAction(act_magic_clear);
    QAction* act_magic_clear_all= createAction(tr("clear all"), "");
    menu_magic->addAction(act_magic_clear_all);
    menu_magic->addSeparator();

    QAction* act_favorites= createAction(tr("magic Commands"), ":/image/image/favorites.png");
    act_favorites->setMenu(menu_magic);
    SARibbonToolButton* btn_favorites = panel_kernel->addLargeAction(act_favorites);
    btn_favorites->setPopupMode(QToolButton::MenuButtonPopup);

    panel_kernel->addSeparator();

    QAction* act_analyze_code = createAction(tr("Analyze Code"), ":/image/image/analysis.png");
    panel_kernel->addSmallAction(act_analyze_code);

    QAction* act_run_and_time = createAction(tr("Run and Time"), ":/image/image/time.png");
    panel_kernel->addSmallAction(act_run_and_time);

    QAction* act_clear_commands = createAction(tr("Clear Commands"), ":/image/image/clear.png");
    act_clear_commands->setMenu(menu);
    act_clear_commands->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = panel_kernel->addSmallAction(act_clear_commands);
    btn->setPopupMode(QToolButton::InstantPopup);


    // panel---Simulink
    SARibbonPannel* panel_simulation = page->addPannel(("Simulation"));
    QAction* act_simulink= createAction(tr("System Simulate"), ":/image/image/endless.png");
    act_simulink->setCheckable(true);
    panel_simulation->addLargeAction(act_simulink);

    panel_simulation->addSeparator();

    QAction* act_add_on = createAction(tr("Hardware Driver"), ":/image/image/ladder.png");
    act_add_on->setMenu(menu);
    btn = panel_simulation->addLargeAction(act_add_on);
    btn->setPopupMode(QToolButton::DelayedPopup);

    // panel---Environment
    SARibbonPannel* panel_env = page->addPannel(("Environment"));

    QAction* act_layout = createAction(tr("Layout"), ":/image/image/layout-one.png");
    act_layout->setMenu(menu);
    btn = panel_env->addLargeAction(act_layout);
    btn->setPopupMode(QToolButton::DelayedPopup);

    panel_env->addSeparator();

    QAction* act_preference = createAction(tr("Preference"), ":/image/image/config.png");
    connect(act_preference, SIGNAL(triggered()), this, SLOT(onPreferenceTriggered()));
    panel_env->addSmallAction(act_preference);

    QAction* act_set_path = createAction(tr("set Path"), ":/image/image/local.png");
    panel_env->addSmallAction(act_set_path);

    QAction* act_parallel = createAction(tr("Parallel"), ":/image/image/list-checkbox.png");
    act_parallel->setMenu(menu);
    act_parallel->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = panel_env->addSmallAction(act_parallel);
    btn->setPopupMode(QToolButton::InstantPopup);


    // panel---Resource
    SARibbonPannel* panel_resource = page->addPannel(("Resource"));

    QAction* act_check_for_update = createAction(tr("check Update"), ":/image/image/refresh.png");
    panel_resource->addLargeAction(act_check_for_update);

    SARibbonMenu* menu_help = new SARibbonMenu(this);
    QAction* act_help_tutorial= createAction(tr("Tutorial"), ":/image/image/book-open.png");
    menu_help->addAction(act_help_tutorial);
    QAction* act_help_example= createAction(tr("Examples"), ":/image/image/examples.png");
    menu_help->addAction(act_help_example);
    QAction* act_website= createAction(tr("Website"), ":/image/image/website.png");
    menu_help->addAction(act_website);
    menu_help->addSeparator();

    QAction* act_help_licensing= createAction(tr("Licensing"), ":/image/image/licencing.png");
    SARibbonMenu* menu_licensing = new SARibbonMenu(this);
    QAction* act_licensing_purchase = createAction("purchase Licenses","");
    menu_licensing->addAction(act_licensing_purchase);
    QAction* act_licensing_donation = createAction("donate or sponsor", "");
    connect(act_licensing_donation, SIGNAL(triggered()), this, SLOT(onDonationOrSponsorTriggered()));
    menu_licensing->addAction(act_licensing_donation);
    menu_licensing->addSeparator();
    QAction* act_licensing_activate = createAction("activate Software","");
    menu_licensing->addAction(act_licensing_activate);
    QAction* act_licensing_deactivate = createAction("deactivate Software","");
    menu_licensing->addAction(act_licensing_deactivate);
    menu_licensing->addSeparator();
    QAction* act_licensing_manage = createAction("manage Licenses","");
    menu_licensing->addAction(act_licensing_manage);

    act_help_licensing->setMenu(menu_licensing);
    menu_help->addAction(act_help_licensing);

    QAction* act_say_thanks = createAction(tr("say Thanks"), ":/image/image/rock-gesture.png");
    connect(act_say_thanks, SIGNAL(triggered()), this, SLOT(onSayThanksTriggered()));
    menu_help->addAction(act_say_thanks);

    QAction* act_participate = createAction(tr("join us"), ":/image/image/participate.png");
    connect(act_participate, SIGNAL(triggered()), this, SLOT(onParticipateTriggered()));
    menu_help->addAction(act_participate);

    menu_help->addSeparator();
    menu_help->addAction(act_check_for_update);
    QAction* act_help_license= createAction(tr("licenses Agreement"), ":/image/image/certificate.png");
    connect(act_help_license, SIGNAL(triggered()), this, SLOT(onLicensesAgreement()));
    menu_help->addAction(act_help_license);
    QAction* act_help_about= createAction(tr("about SCLab"), ":/image/image/info.png");
    connect(act_help_about, SIGNAL(triggered()), this, SLOT(onAboutTriggered()));
    menu_help->addAction(act_help_about);

    QAction* act_help = createAction(tr("Help"), ":/image/image/help.png");
    act_help->setMenu(menu_help);
    btn = panel_resource->addLargeAction(act_help);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    panel_resource->addSeparator();

    QAction* act_community = createAction(tr("Community"), ":/image/image/peoples-two.png");
    panel_resource->addSmallAction(act_community);

    QAction* act_request_support = createAction(tr("request Support"), ":/image/image/teaching.png");
    panel_resource->addSmallAction(act_request_support);

    QAction* act_learn_sclab = createAction(tr("learn SCLab"), ":/image/image/degree-hat.png");
    panel_resource->addSmallAction(act_learn_sclab);

    panel_resource->addSeparator();

    QAction* act_request_feature = createAction(tr("request Features"), ":/image/image/request_feature.png");
    connect(act_request_feature, SIGNAL(triggered()), this, SLOT(onRequestFeaturesTriggered()));
    panel_resource->addLargeAction(act_request_feature);
    QAction* act_report_bug = createAction(tr("report Bugs"), ":/image/image/bug.png");
    connect(act_report_bug, SIGNAL(triggered()), this, SLOT(onReportBugsTriggered()));
    panel_resource->addLargeAction(act_report_bug);

    // panel-communication
    // SARibbonPannel* panel_communication = page->addPannel(("Communication"));
    // QAction* act_request_feature = createAction(tr("request Features"), ":/image/image/request_feature.png");
    // connect(act_request_feature, SIGNAL(triggered()), this, SLOT(onRequestFeaturesTriggered()));
    // panel_communication->addLargeAction(act_request_feature);
    //
    // panel_communication->addSeparator();
    //
    // QAction* act_report_bug = createAction(tr("report Bugs"), ":/image/image/bug.png");
    // connect(act_report_bug, SIGNAL(triggered()), this, SLOT(onReportBugsTriggered()));
    // panel_communication->addLargeAction(act_report_bug);
}

void MainWindow::createCategoryPkg(SARibbonCategory* page)
{
    // panel-python
    SARibbonPannel* panel_python =  page->addPannel(("Python-Pip"));

    auto* act_py_install = new QAction(this);
    act_py_install->setText("install");
    act_py_install->setIcon(QIcon(":/image/image/add-mode.png"));
    act_py_install->setObjectName("install-py");
    panel_python->addLargeAction(act_py_install);

    auto* act_py_uninstall = new QAction(this);
    act_py_uninstall->setText("unInstall");
    act_py_uninstall->setIcon(QIcon(":/image/image/delete-mode.png"));
    act_py_uninstall->setObjectName("uninstall-py");
    panel_python->addLargeAction(act_py_uninstall);

    auto* act_py_upgrade_one = new QAction(this);
    act_py_upgrade_one->setText("upgrade          One");
    act_py_upgrade_one->setIcon(QIcon(":/image/image/update_one.png"));
    act_py_upgrade_one->setObjectName("upgrade-one-py");
    panel_python->addLargeAction(act_py_upgrade_one);

    auto* act_py_upgrade_all = new QAction(this);
    act_py_upgrade_all->setText("upgrade          All");
    act_py_upgrade_all->setIcon(QIcon(":/image/image/update_all.png"));
    act_py_upgrade_all->setObjectName("upgrade-all-py");
    panel_python->addLargeAction(act_py_upgrade_all);

    panel_python->addSeparator();

    auto* act_py_pkg = new QAction(this);
    act_py_pkg->setText("Package Information");
    act_py_pkg->setIcon(QIcon(":/image/image/package.png"));
    act_py_pkg->setObjectName("pkg-info-py");
    panel_python->addSmallAction(act_py_pkg);

    auto* comBox_py_pkg_names = new SARibbonComboBox(this);
    comBox_py_pkg_names->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    comBox_py_pkg_names->setWindowTitle(tr("ComboBox"));
    for (int i = 0; i < 40; ++i) {
        comBox_py_pkg_names->addItem(QString("SARibbonComboBox test%1").arg(i + 1));
    }
    comBox_py_pkg_names->setEditable(false);
    panel_python->addSmallWidget(comBox_py_pkg_names);

    auto* label_py_pkg_version = new QLabel(this);
    label_py_pkg_version->setText(tr("Version: "));
    label_py_pkg_version->setObjectName(QStringLiteral(u"Package-py-Version"));
    QAction* act_label_py_pkg_version = panel_python->addWidget(label_py_pkg_version, SARibbonPannelItem::Small);
    act_label_py_pkg_version->setObjectName(label_py_pkg_version->objectName());

    panel_python->addSeparator();

    QAction* act_pypi = createAction(tr("PyPi"), ":/image/image/pypi.png");
    panel_python->addLargeAction(act_pypi);


    // panel-julia
    SARibbonPannel* panel_julia =  page->addPannel(("Julia-Pkg"));

    auto* act_jl_install = new QAction(this);
    act_jl_install->setText("install");
    act_jl_install->setIcon(QIcon(":/image/image/add-mode.png"));
    act_jl_install->setObjectName("install-jl");
    panel_julia->addLargeAction(act_jl_install);

    auto* act_jl_uninstall = new QAction(this);
    act_jl_uninstall->setText("unInstall");
    act_jl_uninstall->setIcon(QIcon(":/image/image/delete-mode.png"));
    act_jl_uninstall->setObjectName("uninstall-jl");
    panel_julia->addLargeAction(act_jl_uninstall);

    auto* act_jl_upgrade_one = new QAction(this);
    act_jl_upgrade_one->setText("upgrade          One");
    act_jl_upgrade_one->setIcon(QIcon(":/image/image/update_one.png"));
    act_jl_upgrade_one->setObjectName("upgrade-one-jl");
    panel_julia->addLargeAction(act_jl_upgrade_one);

    auto* act_jl_upgrade_all = new QAction(this);
    act_jl_upgrade_all->setText("upgrade          All");
    act_jl_upgrade_all->setIcon(QIcon(":/image/image/update_all.png"));
    act_jl_upgrade_all->setObjectName("upgrade-all-jl");
    panel_julia->addLargeAction(act_jl_upgrade_all);

    panel_julia->addSeparator();

    auto* act_jl_pkg = new QAction(this);
    act_jl_pkg->setText("Package Information");
    act_jl_pkg->setIcon(QIcon(":/image/image/package.png"));
    act_jl_pkg->setObjectName("pkg-info-jl");
    panel_julia->addSmallAction(act_jl_pkg);

    auto* comBox_julia_pkg_names = new SARibbonComboBox(this);
    comBox_julia_pkg_names->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    comBox_julia_pkg_names->setWindowTitle(tr("ComboBox"));
    for (int i = 0; i < 40; ++i) {
        comBox_julia_pkg_names->addItem(QString("SARibbonComboBox test%1").arg(i + 1));
    }
    comBox_julia_pkg_names->setEditable(false);
    panel_julia->addSmallWidget(comBox_julia_pkg_names);

    auto* label_jl_pkg_version = new QLabel(this);
    label_jl_pkg_version->setText(tr("Version: "));
    label_jl_pkg_version->setObjectName(QStringLiteral(u"Package-jl-Version"));
    QAction* act_label_jl_pkg_version = panel_julia->addWidget(label_jl_pkg_version, SARibbonPannelItem::Small);
    act_label_jl_pkg_version->setObjectName(label_jl_pkg_version->objectName());

    panel_julia->addSeparator();

    QAction* act_julia_hub = createAction(tr("Julia      Hub"), ":/image/image/juliahub.png");
    act_julia_hub->setCheckable(true);
    panel_julia->addLargeAction(act_julia_hub);
}

void MainWindow::createCategoryPlots(SARibbonCategory* page)
{
    QList< QAction* > galleryActions;
    auto lambdaCreateGalleryAction = [this](const QString& text, const QString& iconurl) -> QAction* {
        QAction* act = this->createAction(text, iconurl);
        return act;
    };

    // panel---selection
    SARibbonPannel* panel_selection = page->addPannel(("Selection"));

    QLabel* label_select_variables = new QLabel(this);
    label_select_variables->setText(tr("No Variable Selected"));
    label_select_variables->setObjectName(QStringLiteral(u"No-Variable-Selected"));
    QAction* act_select_variables = panel_selection->addWidget(label_select_variables, SARibbonPannelItem::Small);
    act_select_variables->setObjectName(label_select_variables->objectName());


    // panel---plots
    SARibbonPannel* panel_plots = page->addPannel(("Plots"));

    SARibbonGallery* gallery_plots = panel_plots->addGallery();
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("plot", ":/gallery/gallery/plot.png"));
    galleryActions.append(lambdaCreateGalleryAction("plot series", ":/gallery/gallery/graph.png"));
    galleryActions.append(lambdaCreateGalleryAction("scatter", ":/gallery/gallery/scatter.png"));
    galleryActions.append(lambdaCreateGalleryAction("scatter-3D", ":/gallery/gallery/scatter3d.png"));
    galleryActions.append(lambdaCreateGalleryAction("stem", ":/gallery/gallery/stem.png"));
    galleryActions.append(lambdaCreateGalleryAction("bar", ":/gallery/gallery/bar.png"));
    galleryActions.append(lambdaCreateGalleryAction("surf", ":/gallery/gallery/surf.png"));
    galleryActions.append(lambdaCreateGalleryAction("mesh", ":/gallery/gallery/mesh.png"));
    galleryActions.append(lambdaCreateGalleryAction("pie", ":/gallery/gallery/pie.png"));
    galleryActions.append(lambdaCreateGalleryAction("polar", ":/gallery/gallery/polar.png"));
    galleryActions.append(lambdaCreateGalleryAction("histogram", ":/gallery/gallery/histogram.png"));
    galleryActions.append(lambdaCreateGalleryAction("contour", ":/gallery/gallery/contour.png"));
    SARibbonGalleryGroup* group_plots = gallery_plots->addCategoryActions(tr("Science Computation"), galleryActions);
    group_plots->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    // group_plots->setGridMinimumWidth(60);
    gallery_plots->setCurrentViewGroup(group_plots);

    // panel---options
    SARibbonPannel* panel_options = page->addPannel(("Options"));

    QButtonGroup* btn_group_options = new QButtonGroup(page);

    QRadioButton* rbtn_reuse_figure = new QRadioButton();
    rbtn_reuse_figure->setText(tr("Reuse Figure"));
    rbtn_reuse_figure->setObjectName(("Reuse Figure"));
    rbtn_reuse_figure->setChecked(true);
    panel_options->addSmallWidget(rbtn_reuse_figure);
    btn_group_options->addButton(rbtn_reuse_figure, SARibbonBar::WpsLiteStyle);

    QRadioButton* rbtn_new_figure = new QRadioButton();
    rbtn_new_figure->setText(tr("New Figure"));
    rbtn_new_figure->setObjectName(("New Figure"));
    rbtn_new_figure->setChecked(true);
    panel_options->addSmallWidget(rbtn_new_figure);
    btn_group_options->addButton(rbtn_new_figure, SARibbonBar::WpsLiteStyle);
}


void MainWindow::createCategoryApps(SARibbonCategory* page)
{
    QList< QAction* > galleryActions;
    auto lambdaCreateGalleryAction = [this](const QString& text, const QString& iconurl) -> QAction* {
        QAction* act = this->createAction(text, iconurl);
        return act;
    };

    // panel---App Management
    SARibbonPannel* panel_manage = page->addPannel(("Apps Management"));

    QAction* act_design= createAction(tr("design            App"), ":/image/image/doc-add.png");
    act_design->setCheckable(true);
    panel_manage->addLargeAction(act_design);

    QAction* act_install_app= createAction(tr("add         InLine"), ":/image/image/doc-search.png");
    act_install_app->setCheckable(true);
    panel_manage->addLargeAction(act_install_app);

    QAction* act_get_more_app= createAction(tr("add       OffLine"), ":/image/image/doc-fail.png");
    act_get_more_app->setCheckable(true);
    panel_manage->addLargeAction(act_get_more_app);

    QAction* act_require_app= createAction(tr("order          App"), ":/image/image/order.png");
    act_require_app->setCheckable(true);
    panel_manage->addLargeAction(act_require_app);


    // panel---Science Computation
    SARibbonPannel* panel_sc = page->addPannel(("Science Computation"));

    SARibbonGallery* gallery_sc = panel_sc->addGallery();

    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("Optimization", ":/gallery/gallery/opt.png"));
    galleryActions.append(lambdaCreateGalleryAction("Curve Fitting", ":/gallery/gallery/curve_fitting.png"));
    SARibbonGalleryGroup* group_sc = gallery_sc->addCategoryActions(tr("Science Computation"), galleryActions);
    group_sc->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group_sc->setGridMinimumWidth(80);
    group_sc->setWrapping(true);
    group_sc->setWordWrap(true);
    gallery_sc->setCurrentViewGroup(group_sc);


    // panel---dsp
    SARibbonPannel* panel_dsp = page->addPannel(("Digital Signal Process"));

    SARibbonGallery* gallery_dsp = panel_dsp->addGallery();
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("Data Signal Process", ":/gallery/gallery/dsp.png"));
    galleryActions.append(lambdaCreateGalleryAction("Image Signal Process", ":/gallery/gallery/image.png"));
    SARibbonGalleryGroup* group_dsp = gallery_dsp->addCategoryActions(tr("Digital Signal Process"), galleryActions);
    group_dsp->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group_dsp->setGridMinimumWidth(85);
    group_dsp->setWordWrap(true);
    group_dsp->setWrapping(true);
    gallery_dsp->setCurrentViewGroup(group_dsp);


    // panel---Control
    SARibbonPannel* panel_control = page->addPannel(("Control System Development"));

    SARibbonGallery* gallery_control = panel_control->addGallery();
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("PID Tuner", ":/gallery/gallery/pid.png"));
    galleryActions.append(lambdaCreateGalleryAction("Transform Function", ":/gallery/gallery/transform.png"));
    galleryActions.append(lambdaCreateGalleryAction("State Space", ":/gallery/gallery/state_space.png"));
    SARibbonGalleryGroup* group_control = gallery_control->addCategoryActions(tr("Digital Signal Process"), galleryActions);
    group_control->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group_control->setGridMinimumWidth(85);
    group_control->setWordWrap(true);
    group_control->setWrapping(true);
    gallery_control->setCurrentViewGroup(group_control);


    // panel---Automatic Drive
    SARibbonPannel* panel_automatic_drive = page->addPannel(("Automatic Drive"));

    SARibbonGallery* gallery_automatic_drive= panel_automatic_drive->addGallery();
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("Lidar", ":/gallery/gallery/lidar.png"));
    galleryActions.append(lambdaCreateGalleryAction("Image", ":/gallery/gallery/image.png"));
    galleryActions.append(lambdaCreateGalleryAction("Radar", ":/gallery/gallery/radar.png"));
    galleryActions.append(lambdaCreateGalleryAction("Slam", ":/gallery/gallery/slam.png"));
    galleryActions.append(lambdaCreateGalleryAction("Perception", ":/gallery/gallery/perception.png"));
    galleryActions.append(lambdaCreateGalleryAction("Planning", ":/gallery/gallery/planning.png"));
    galleryActions.append(lambdaCreateGalleryAction("Controller", ":/gallery/gallery/controller.png"));
    galleryActions.append(lambdaCreateGalleryAction("Ros2", ":/gallery/gallery/robot.png"));
    SARibbonGalleryGroup* group_automatic_drive = gallery_automatic_drive->addCategoryActions(tr("Digital Signal Process"), galleryActions);
    group_automatic_drive->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group_automatic_drive->setGridMinimumWidth(80);
    group_automatic_drive->setWordWrap(true);
    group_automatic_drive->setWrapping(true);
    gallery_automatic_drive->setCurrentViewGroup(group_automatic_drive);
    gallery_automatic_drive->setMinimumWidth(550);
}

void MainWindow::onHideTriggered(bool status_hide)
{
    this->ribbonBar()->setMinimumMode(status_hide);
    if (status_hide){
        act_hide_->setIcon(QIcon(":/image/image/arrow-down3-white.png"));
    }else{
        act_hide_->setIcon(QIcon(":/image/image/arrow-up3-white.png"));
    }
}

void MainWindow::createQuickAccessBar(SARibbonQuickAccessBar* quickAccessBar)
{
    QLabel* label_sw_name = new QLabel(this);
    label_sw_name->setText(sw_version_);
    // label_sw_name->setAlignment(Qt::AlignLeft);
    label_sw_name->setStyleSheet("QLabel{background-color:white;font-size: 14px; color: black}");
    quickAccessBar->addWidget(label_sw_name);
}


void MainWindow::createRightButtonGroup(SARibbonButtonGroupWidget* rightBar)
{
    QAction* act_new = createAction("new", ":/image/image/file-editing-one-white.png", "new-general-file");

    rightBar->addAction(act_new);
    QAction* act_save = createAction("save", ":/image/image/save-white.png", "save");
    rightBar->addAction(act_save);
    rightBar->addSeparator();

    rightBar->addAction(createAction("copy", ":/image/image/copy-white.png"));
    rightBar->addAction(createAction("cut", ":/image/image/cut-white.png"));
    rightBar->addAction(createAction("paste", ":/image/image/paste-white.png"));

    rightBar->addSeparator();
    rightBar->addAction(createAction("undo", ":/image/image/undo-white.png"));
    rightBar->addAction(createAction("redo", ":/image/image/redo-white.png"));
    // rightBar->addSeparator();
    // QMenu* m = new QMenu("Presentation File", this);
    // m->setIcon(QIcon(":/image/image/presentationFile-white.png"));
    // for (int i = 0; i < 10; ++i) {
    //     m->addAction(createAction(QString("file%1").arg(i + 1), ":/image/image/file.png"));
    // }
    // rightBar->addMenu(m);

    rightBar->addSeparator();

    auto lineEdit_search = new SARibbonLineEdit(this);
    lineEdit_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit_search->setPlaceholderText("Search Features");
    lineEdit_search->setObjectName("lineEdit_search");
    lineEdit_search->setStyleSheet("background:white;color:black");
    auto* act_lineEdit_search = new QAction(lineEdit_search);
    act_lineEdit_search->setIcon(QIcon(":/image/image/search.png"));
    lineEdit_search->addAction(act_lineEdit_search, QLineEdit::LeadingPosition);
    lineEdit_search->setMinimumWidth(150);
    rightBar->addWidget(lineEdit_search);

    rightBar->addSeparator();

    act_hide_ = createAction(tr("Hide"), ":/image/image/arrow-up3-white.png");
    act_hide_->setCheckable(true);
    connect(act_hide_, &QAction::triggered, this, &MainWindow::onHideTriggered);
    rightBar->addAction(act_hide_);

    QAction* act_info = createAction(tr("Notifications"), ":/image/image/remind-white.png");
    connect(act_info, &QAction::triggered, this, &MainWindow::onNotificationsTriggered);
    rightBar->addAction(act_info);


    QAction* act_sign_in = createAction("Sign-In", ":/image/image/sign-in-white.png");
    act_sign_in->setText("Sign In");
    rightBar->addAction(act_sign_in);

    rightBar->setStyleSheet("background:#2b579a};color:black");
}

void MainWindow::addSomeOtherAction()
{
    //添加其他的action，这些action并不在ribbon管理范围，主要用于SARibbonCustomizeWidget自定义用
    QAction* acttext1 = new QAction(("text action1"), this);
    QAction* acttext2 = new QAction(("text action2"), this);
    QAction* acttext3 = new QAction(("text action3"), this);
    QAction* acttext4 = new QAction(("text action4"), this);
    QAction* acttext5 = new QAction(("text action5"), this);

    QAction* actIcon1 = new QAction(QIcon(":/icon/icon/layout.svg"), ("action with icon"), this);

    m_actionTagText     = SARibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = SARibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new SARibbonActionsManager(this);  //申明过程已经自动注册所有action

    //以下注册特别的action
    m_actMgr->registeAction(acttext1, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext3, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext5, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(actIcon1, SARibbonActionsManager::CommonlyUsedActionTag);

    m_actMgr->registeAction(acttext1, m_actionTagText);
    m_actMgr->registeAction(acttext2, m_actionTagText);
    m_actMgr->registeAction(acttext3, m_actionTagText);
    m_actMgr->registeAction(acttext4, m_actionTagText);
    m_actMgr->registeAction(acttext5, m_actionTagText);

    m_actMgr->registeAction(actIcon1, m_actionTagWithIcon);

    m_actMgr->setTagName(SARibbonActionsManager::CommonlyUsedActionTag, ("常用"));
    m_actMgr->setTagName(m_actionTagText, ("无图标action"));
    m_actMgr->setTagName(m_actionTagWithIcon, ("有图标action"));
}


void MainWindow::createContextCategory1()
{
    //上下文标签
    SARibbonBar* ribbon = ribbonBar();
    m_contextCategory   = ribbon->addContextCategory(tr("context"), QColor(), 1);

    SARibbonCategory* contextCategoryPage1 = m_contextCategory->addCategoryPage(tr("Page1"));
    createContextCategoryPage1(contextCategoryPage1);

    SARibbonCategory* contextCategoryPage2 = m_contextCategory->addCategoryPage(tr("Page2"));
    createContextCategoryPage2(contextCategoryPage2);
}

void MainWindow::createContextCategory2()
{
    SARibbonBar* ribbon = ribbonBar();
    m_contextCategory2  = ribbon->addContextCategory(("context2"), QColor(), 2);
    m_contextCategory2->addCategoryPage(("context2 Page1"));
    m_contextCategory2->addCategoryPage(("context2 Page2"));
}

void MainWindow::createContextCategoryPage1(SARibbonCategory* page)
{
    /*
    SARibbonPannel* pannel = page->addPannel(tr("show and hide test"));

    QAction* actionHidePannel = createAction("hide pannel", ":/icon/icon/hidePannel.svg");
    actionHidePannel->setCheckable(true);
    pannel->addLargeAction(actionHidePannel);

    QAction* actionDisable = createAction(tr("Disable"), ":/icon/icon/enableTest.svg");

    actionDisable->setDisabled(true);
    pannel->addLargeAction(actionDisable);
    connect(actionDisable, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setDisabled(true);
    });

    QAction* actionUnlock = createAction(tr("unlock"), ":/icon/icon/unlock.svg");
    actionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    pannel->addLargeAction(actionUnlock);
    connect(actionUnlock, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setEnabled(true);
        actionDisable->setText(("Enabled"));
    });

    QAction* actionSetTextTest = createAction("set text", ":/icon/icon/setText.svg");

    actionSetTextTest->setCheckable(true);
    actionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    pannel->addLargeAction(actionSetTextTest);

    connect(actionSetTextTest, &QAction::toggled, this, [actionSetTextTest](bool b) {
        if (b) {
            actionSetTextTest->setText(QStringLiteral(u"setText测试"));
        } else {
            actionSetTextTest->setText(QStringLiteral(u"set text"));
        }
    });
    //隐藏pannel
    QAction* actionShowTest = createAction("show beside pannel", ":/icon/icon/show.svg");

    actionShowTest->setCheckable(true);
    pannel->addLargeAction(actionShowTest);

    SARibbonPannel* pannel2 = page->addPannel(tr("show/hide"));

    pannel2->addLargeAction(actionSetTextTest);

    connect(actionShowTest, &QAction::toggled, this, [actionShowTest, pannel2, this](bool b) {
        pannel2->setVisible(!b);
        if (b) {
            actionShowTest->setText(tr("hide beside pannel"));
        } else {
            actionShowTest->setText(tr("show beside pannel"));
        }
        ribbonBar()->repaint();
    });

    SARibbonPannel* pannel3 = page->addPannel(("show/hide action test"));

    QAction* actionHideAction2 = createAction("hide action 2", ":/icon/icon/action.svg");

    actionHideAction2->setCheckable(true);
    actionHideAction2->setChecked(true);

    QAction* act2 = createAction("action2", ":/icon/icon/action2.svg");
    QAction* act3 = createAction("action3", ":/icon/icon/action3.svg");
    QAction* act4 = createAction("action4", ":/icon/icon/action4.svg");
    pannel3->addLargeAction(actionHideAction2);
    pannel3->addSmallAction(act2);
    pannel3->addSmallAction(act3);
    pannel3->addSmallAction(act4);
    connect(actionHideAction2, &QAction::triggered, this, [actionHideAction2, act2](bool b) {
        if (b) {
            act2->setVisible(true);
            actionHideAction2->setText(tr("hide action2"));
        } else {
            act2->setVisible(false);
            actionHideAction2->setText(tr("show action2"));
        }
    });
    */
}

void MainWindow::createContextCategoryPage2(SARibbonCategory* page)
{
    SARibbonPannel* pannel1 = page->addPannel(("删除CategoryPage测试"));

    QAction* actionDeleteThisCategory = createAction("delete this category", ":/icon/icon/delete.svg");
    pannel1->addLargeAction(actionDeleteThisCategory);
    connect(actionDeleteThisCategory, &QAction::triggered, this, [this, page]() {
        this->ribbonBar()->removeCategory(page);
        page->deleteLater();
    });
    SARibbonPannel* pannelLayout = page->addPannel(("特殊布局"));

    pannelLayout->addAction(createAction("Large", ":/icon/icon/layout.svg", "@Large1"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small1"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small2"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addSeparator();
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small3"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small4"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small5"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small6"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addSeparator();
    pannelLayout->addAction(createAction("Large", ":/icon/icon/layout.svg", "@Large2"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium1"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium2"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small7"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium3"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Large", ":/icon/icon/layout.svg", "@Large3"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium4"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium5"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Large", ":/icon/icon/layout.svg", "@Large4"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannelLayout->addSeparator();
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium6"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Large", ":/icon/icon/layout.svg", "@Large5"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium7"),
                            QToolButton::InstantPopup,
                            SARibbonPannelItem::Medium);
    pannelLayout->addAction(createAction("Small", ":/icon/icon/layout.svg", "@Small8"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
}

QAction* MainWindow::createAction(const QString& text, const QString& iconurl, const QString& objName)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(objName);
    return act;
}

QAction* MainWindow::createAction(const QString& text, const QString& iconurl)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    return act;
}

void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(QString("InstantPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("DelayedPopupCheckabletriggered : %1").arg(b));
}

// close all kernels
void MainWindow::closeEvent(QCloseEvent *event)
{
    // emit quitWindow();
    widget_center_->stopChannels();
    widget_center_->shutDownKernels();
}

// The function below not implement
void MainWindow::shutDownKernelsOnExit() {
    // qDebug() << "About to quit!";
}

void MainWindow::onAboutTriggered() {
    auto dlg_about_sclab = new AboutSClab();
    int ret = dlg_about_sclab->exec();
    if (ret==QDialog::Accepted){
        // qDebug() << dlg_about_sclab->getStatement();
    }else{
        // qDebug() << dlg_about_sclab->getStatement();
    }
    delete dlg_about_sclab;
}

void MainWindow::onRequestFeaturesTriggered() {
    QMessageBox box;
    box.setWindowTitle("Request Features");
    box.setWindowIcon(QIcon(":/image/image/sc_v11.png"));
    box.setText("Please send an e-mail to sclab2022@outlook.com.");
    box.exec();
}

void MainWindow::onReportBugsTriggered() {
    QMessageBox box;
    box.setWindowTitle("Report Bugs");
    box.setWindowIcon(QIcon(":/image/image/sc_v11.png"));
    box.setText("Please send an e-mail to sclab2022@outlook.com.");
    box.exec();
}

void MainWindow::onParticipateTriggered(){
    QMessageBox box;
    box.setWindowTitle("Participate Development");
    box.setWindowIcon(QIcon(":/image/image/sc_v11.png"));
    box.setText("Thanks for your participation.\nPlease send an e-mail to sclab2022@outlook.com.");
    box.exec();
}

void MainWindow::onSayThanksTriggered() {
    auto dlg_say_thanks = new SayThanks();
    int ret = dlg_say_thanks->exec();
    if (ret==QDialog::Accepted){
        ;
    }else{
        ;
    }
    delete dlg_say_thanks;
}

void MainWindow::onDonationOrSponsorTriggered(){
    auto dlg_donation_or_sponsor = new DonationOrSponsor();
    int ret = dlg_donation_or_sponsor->exec();
    if (ret==QDialog::Accepted){
        ;
    }else{
        ;
    }
    delete dlg_donation_or_sponsor;
}

void MainWindow::onLicensesAgreement() {
    auto dlg_license_agreement = new LicensesAgreement();
    int ret = dlg_license_agreement->exec();
    if (ret == QDialog::Accepted) { ;
    } else { ;
    }
    delete dlg_license_agreement;
}

void MainWindow::onPreferenceTriggered() {
    auto dlg_preference = new Preferences();
    int ret = dlg_preference->exec();
    if (ret==QDialog::Accepted){
        ;
    } else{
        ;
    }
    delete dlg_preference;
}