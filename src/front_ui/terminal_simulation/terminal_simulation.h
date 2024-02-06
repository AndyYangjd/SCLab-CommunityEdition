#ifndef SCLAB_TERMINAL_SIMULATION_H
#define SCLAB_TERMINAL_SIMULATION_H

#include "ui_terminal_simulation.h"
#include <QVector>
#include <QUuid>
#include <QWidget>
#include <QSysInfo>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QToolButton>

#include "small_widget_center/welcome_home/welcome_home.h"
#include "small_widget_center/kernel_selection/kernel_selection.h"
#include "kernel_widget/jupyter_widget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TerminalSimulation; }
QT_END_NAMESPACE

class TerminalSimulation : public QWidget {
Q_OBJECT

public:
    explicit TerminalSimulation(QWidget *parent = nullptr);

    ~TerminalSimulation() override;

    void stopChannels();
    void shutDownKernels();

private:
    Ui::TerminalSimulation *ui_;

    QToolButton* toolBtn_{};
    QString new_kernel_name_{};
    unsigned int num_kernel_{0u};

    void initSelfDefinedUi();
    void initConnections();
    void createHomeWidget();
    // QWidget* createKernelWidget();

    KernelManager* jl_kernel_manager_{nullptr};
    KernelManager* py_kernel_manager_{nullptr};
    QVector<KernelClient*> vec_jl_kernel_client_{};
    QVector<KernelClient*> vec_py_kernel_client_{};
    void createKernelManagerAndClient();

    const int MAX_NUM_KERNELS{6};
    QString project_path_{};
    QStringList connection_file_list_{};
    void generateConnectionFile();
signals:
    void sig_changeCursorPos(int, int);
private slots:
    void tapClosed(int);
    void createNewTab();
    void getCursorPosition(int id_row, int id_col);
};


#endif //SCLAB_TERMINAL_SIMULATION_H
