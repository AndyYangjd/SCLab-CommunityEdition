#ifndef SCLAB_WIDGET_CENTER_H
#define SCLAB_WIDGET_CENTER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include "ui_widget_center.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetCenter; }
QT_END_NAMESPACE

class WidgetCenter : public QWidget {
Q_OBJECT

public:
    explicit WidgetCenter(QWidget *parent = nullptr);

    ~WidgetCenter() override;

    void stopChannels();
    void shutDownKernels();
private:
    Ui::WidgetCenter *ui_;

    QString cur_path_;

    QFileSystemModel* treeModel_file_;
    QStandardItemModel* tabModel_var_space_{};
    QStandardItemModel* tabModel_fn_space_{};

    void initSelfDefinedUi();
    void initTreeView();
    void initTableView();
    void initConnections();

signals:
    void sig_createNewTab();
    void sig_changeCursor(int, int);
private slots:
    void clickTabBar(int id);
    void triggerNewTab();
    void getCursor(int, int);
};


#endif //SCLAB_WIDGET_CENTER_H
