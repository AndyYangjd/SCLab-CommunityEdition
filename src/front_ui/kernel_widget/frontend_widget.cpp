#include "frontend_widget.h"

FrontendWidget::FrontendWidget(QWidget *parent1,BaseFrontendMixin* parent2) :
        HistoryConsoleWidget(parent1), BaseFrontendMixin() {
}

FrontendWidget::~FrontendWidget() {

}

void FrontendWidget::initVars(){
    setContinuationPrompt("... ", false);

    auto document = control_->document();
    connect(document, SIGNAL(contentsChange(int, int, int)), this, SLOT(changDocumentContent()));
}

// Called whenever the document's content changes. Display a call tip if appropriate.
void FrontendWidget::changDocumentContent(int _pos, const int _rm, const int _add) {
    _pos+=_add;

    // doc = control_->document();
    if (_pos == getCursor().position()){
        callTipAuto();
    }
}

void FrontendWidget::callTipAuto(){

}

void FrontendWidget::reset(bool is_clear) {
    if (is_executing_){
        is_executing_ = false;
    }
    is_reading_= false;

    if(is_clear){
        control_->clear();
        if (is_display_banner_){
            if (kernel_name_=="julia"){
                appendPlainText(banner_jl_);
            }else if (kernel_name_=="python"){
                appendPlainText(banner_py_);
            }else if (kernel_name_=="blend"){
                ;
            }else{
                ;
            }
        }
    }

    showInterpreterPrompt();
}

void FrontendWidget::startChannels() {
    reset(true);
}
