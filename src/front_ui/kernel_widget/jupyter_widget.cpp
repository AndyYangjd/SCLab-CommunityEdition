#include "jupyter_widget.h"


JupyterWidget::JupyterWidget(QWidget *parent) :
        FrontendWidget(parent){

}

JupyterWidget::~JupyterWidget() {
}

void JupyterWidget::setKernelName(const QString & _kernel_name) {
    kernel_name_ = _kernel_name;
}

void JupyterWidget::setKernelManager(KernelManager *cur_kernel_manager) {
    kernel_manager_ = cur_kernel_manager;
}

void JupyterWidget::setKernelClient(KernelClient *cur_kernel_client) {
    kernel_client_ = cur_kernel_client;
}

void JupyterWidget::startChannels() {
    is_starting_ = true;
}

QString JupyterWidget::makeContinuationPrompt(const QString & prompt, bool is_remote) {
    QString str_end = "...: ";

    return str_end;
}

void JupyterWidget::processExecutePayload(const QString& item) {

}

void JupyterWidget::processExecuteError(const QString &msg) {

}


void JupyterWidget::handleKernelInfoReply(const QString &reply) {

}

void JupyterWidget::handleDisplayData(const QString &msg) {

}

void JupyterWidget::handleExecuteResult(const QString& msg){

}

void JupyterWidget::handleExecuteInput(const QString&) {

}

void JupyterWidget::insertOtherInput(QTextCursor cursor, const QString & content, bool is_remote) {

}

void JupyterWidget::handleHistoryReply(const QString&) {

}

void JupyterWidget::handleExecuteReply(const QString&) {

}

void JupyterWidget::handleCompleteReply(const QString&) {

}

void JupyterWidget::updatePrompt(const int new_number) {

}

void JupyterWidget::showInterpreterPromptForReply(const QString &msg) {

}

void JupyterWidget::showInterpreterPrompt(const int number) {
    prompt_sep_ = default_in_prompt_;
    showPrompt(prompt_sep_);

    QTextBlock block = control_->document()->lastBlock();
    int len_prompt= prompt_.length();

    previous_prompt_info_.block = block;
    previous_prompt_info_.len=len_prompt;
    previous_prompt_info_.number=number;

    QString str_next = makeContinuationPrompt(prompt_);
    setContinuationPrompt(str_next, false);
}

