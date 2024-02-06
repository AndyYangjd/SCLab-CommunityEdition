#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "SARibbon/SARibbonMainWindow.h"
#include "center_widget/widget_center.h"

#include "status_bar/status_bar.h"
#include "small_widget_ribbon/about_sclab/about_sclab.h"
#include "small_widget_ribbon/donation_or_sponsor/donation_or_sponsor.h"
#include "small_widget_ribbon/licenses_agreement/licenses_agreement.h"
#include "small_widget_ribbon/say_thanks/say_thanks.h"
#include "small_widget_ribbon/preferences/preferences.h"

class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class SARibbonActionsManager;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class QTextEdit;

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par = nullptr);

private:
    const QString sw_type_{"Community   Edition"};
    const QString sw_version_{"SCLab R2022     "};

    QAction * act_hide_{};
    WidgetCenter* widget_center_{nullptr};
    QStatusBar* status_bar_{nullptr};

    void createCategoryHome(SARibbonCategory* page);
    void createCategoryPkg(SARibbonCategory* page);
    void createCategoryPlots(SARibbonCategory* page);
    void createCategoryApps(SARibbonCategory* page);
    void createQuickAccessBar(SARibbonQuickAccessBar* quickAccessBar);
    void createRightButtonGroup(SARibbonButtonGroupWidget* rightBar);
    void addSomeOtherAction();
    void createContextCategory1();
    void createContextCategory2();
    void createContextCategoryPage1(SARibbonCategory* page);
    void createContextCategoryPage2(SARibbonCategory* page);
    QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);
    QAction* createAction(const QString& text, const QString& iconurl);
private:
    SARibbonContextCategory* m_contextCategory;
    SARibbonContextCategory* m_contextCategory2;
    SARibbonCustomizeWidget* m_customizeWidget;
    QTextEdit* m_edit;
    SARibbonActionsManager* m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void quitWindow();

private slots:
    void onAboutTriggered();
    void onNotificationsTriggered();
    void onRequestFeaturesTriggered();
    void onReportBugsTriggered();
    void onHideTriggered(bool);
    void onParticipateTriggered();
    void onSayThanksTriggered();
    void onDonationOrSponsorTriggered();
    void onLicensesAgreement();
    void onPreferenceTriggered();
    void shutDownKernelsOnExit();
    //-----------------------------
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);
    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);
    void onStyleClicked(int id);
    void onActionCustomizeTriggered(bool b);
    void onActionCustomizeAndSaveTriggered(bool b);
    void onActionRemoveAppBtnTriggered(bool b);
    void onActionUseQssTriggered();
    void onActionLoadCustomizeXmlFileTriggered();
    void onActionWindowFlagNormalButtonTriggered(bool b);

    void onFontComWidgetCurrentFontChanged(const QFont& f);
    void onActionFontLargerTriggered();
    void onActionFontSmallerTriggered();
    void onActionwordWrapIn2rowTriggered(bool b);
};

#endif  // MAIN_WINDOW_H
