#ifndef SCLAB_JUPYTER_WIDGET_H
#define SCLAB_JUPYTER_WIDGET_H

#include "kernel_widget/frontend_widget.h"
#include "kernel_widget/kernel_manager.h"
#include "kernel_widget/kernel_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class JupyterWidget; }
QT_END_NAMESPACE

class JupyterWidget : public FrontendWidget {
    Q_OBJECT

public:
    explicit JupyterWidget(QWidget *parent = nullptr);

    ~JupyterWidget() override;

    void setKernelName(const QString&);
    void setKernelManager(KernelManager* cur_kernel_manager);
    void setKernelClient(KernelClient* cur_kernel_client);
private:
    // kernel_manager_ is attached to this tap
    KernelManager* kernel_manager_{nullptr};
    // kernel_client_ is attached to this tap
    KernelClient* kernel_client_{nullptr};

protected:
    typedef struct {
        QTextBlock block;
        int len;
        int number;
    }PromptBlockInfo_t;

    const QString default_in_prompt_ = ">>";
    const QString default_out_prompt_ = "";
    const QString default_in_sep_ = "\n";
    const QString default_out_sep_ = "";
    const QString default_out_sep2_ = "";

    PromptBlockInfo_t previous_prompt_info_{};

    bool is_request_prompt_{false};
    bool is_starting_{false};

    // jupyter-widget interface
    void setDefaultStyle(const QString& colors="light"){};

    // jupyter-widget protected interface
    void edit(const QString& file_name, int line){};
    void makePromptInput(){};
    QString makeContinuationPrompt(const QString&, bool is_remote=false);
    void makePromptOutput(){};

    // payload handlers
    void handlePayloadEdit(){};
    void handlePayloadExit(){};
    void handlePayloadNextInput(){};
    void handlePayloadPage(){};

    // BaseFrontendMixin abstract interface
    void handleCompleteReply(const QString& rep);
    void handleExecuteReply(const QString& msg);
    void handleHistoryReply(const QString& msg);
    void insertOtherInput(QTextCursor cursor, const QString& content, bool is_remote=false);
    void handleExecuteInput(const QString& msg);
    void handleExecuteResult(const QString& msg);
    void handleDisplayData(const QString& msg);
    void handleKernelInfoReply(const QString& reply);
    void startChannels() override;

    // frontend-widget protected interface
    void processExecuteError(const QString& msg);
    void processExecutePayload(const QString& item);
    void showInterpreterPrompt(int number=0);
    void updatePrompt(const int new_number=0);
    void showInterpreterPromptForReply(const QString& msg);
};


#endif //SCLAB_JUPYTER_WIDGET_H
