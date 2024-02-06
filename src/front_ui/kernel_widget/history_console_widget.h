#ifndef SCLAB_HISTORY_CONSOLE_WIDGET_H
#define SCLAB_HISTORY_CONSOLE_WIDGET_H

#include "console_widget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class HistoryConsoleWidget; }
QT_END_NAMESPACE

class HistoryConsoleWidget : public ConsoleWidget {
    Q_OBJECT

public:
    explicit HistoryConsoleWidget(QWidget *parent = nullptr);

    ~HistoryConsoleWidget() override;

protected:
    QStringList history_{};
    QStringList history_edit_{};
    int history_index_{0};
    QString history_prefix{""};

    void doExecute();
    void pressUp();
    void pressDown();
    void getPreviousHistory();
    void getNextHistory();
    void getTailHistory();

    bool isHistoryLocked();
    void getEditedHistory();
    void setHistory();
    void storeEdits();
};


#endif //SCLAB_HISTORY_CONSOLE_WIDGET_H
