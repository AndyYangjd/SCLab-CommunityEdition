#ifndef SCLAB_FRONTEND_WIDGET_H
#define SCLAB_FRONTEND_WIDGET_H

#include "history_console_widget.h"
#include "base_frontend_mixin.h"


QT_BEGIN_NAMESPACE
namespace Ui { class FrontendWidget; }
QT_END_NAMESPACE

class FrontendWidget : public HistoryConsoleWidget, public BaseFrontendMixin{
    Q_OBJECT

public:
    explicit FrontendWidget(QWidget *parent1 = nullptr, BaseFrontendMixin* parent2= nullptr);

    ~FrontendWidget() override;
    void reset(bool is_clear=false);
protected:
    QString kernel_name_{""};
    QString banner_jl_{"Welcome to SCLab-R2022b with the kernel of Julia-v1.7.2."};
    QString banner_py_{"Welcome to SCLab-R2022b with the kernel of Python-v3.9.6."};
    bool is_display_banner_{true};
    // Whether to draw information call-tips on open-parentheses.
    bool is_enable_call_tips{true};
    // Whether to clear the console when the kernel is restarted
    bool is_clear_restart{true};
    // Whether to ask for user confirmation when restarting kernel
    bool is_confirm_restart{true};

    bool is_local_kernel{true};

    bool is_pending_clear_output{false};

    // frontend-widget public interface
    void copyRaw(){};
    void interruptKernel(){};

    void restartKernel(const QString& msg, bool is_now=false){};
    void appendStream(const QString& text){};
    void flushClearOutput(){};
    void clearOutput(){};
    // fronted-widget protected interface
    void callTipAuto();
    void callTip();
    void complete(){};
    void processExecuteAbort(const QString& msg){};
    void processExecuteError(const QString& msg){};
    void processExecuteOk(const QString& msg){};
    bool processExecutePayload(){return false;}
    void showInterpreterPrompt(){ showPrompt(">> ");};
    void showInterpreterPromptForReply(){};

    // console-widget public interface
    void copy(){};

    // console-widget abstract interface
    void execute(const QString& source ,bool is_hidden){};
    void startPromptHook(){};
    void finishPromptHook(){};
    void pressTab(){};

    // console-widget protected interface
    void makeContextMenu(){};
    void requestInterruptKernel(){};
    void requestRestartKernel(){};
    void keypressEventFilterConsole(){};

    // BaseFrontendMixin abstract interface
    void handleClearOutput(const QString&){};
    void silentExecCallback(){};
    void handleExecCallback(){};
    void handleExecuteReply(){};
    void handleError(const QString& msg){};
    void handleInputRequest(){};
    void restartKernelMessage(const QString& msg){};
    void handleKernelDied(){};
    void handleKernelRestart(){};
    void handleInspectReply(){};
    void handleExecuteResult(){};
    void handleStream(){};
    void handleShutdownReply(){};
    void handleStatus(const QString& msg){};
    virtual void startChannels();

private:
    void initVars();

protected slots:
    void changDocumentContent(int, const int, const int);

signals:
    void executing();
    void executed();
    void requestExit();
};


#endif //SCLAB_FRONTEND_WIDGET_H
