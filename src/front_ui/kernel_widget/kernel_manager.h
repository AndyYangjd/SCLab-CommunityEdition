#ifndef SCLAB_KERNEL_MANAGER_H
#define SCLAB_KERNEL_MANAGER_H

#include <QWidget>
#include "kernel_client.h"


QT_BEGIN_NAMESPACE
namespace Ui { class KernelManager; }
QT_END_NAMESPACE

class KernelManager : public QWidget {
Q_OBJECT

public:
    explicit KernelManager(QWidget *parent = nullptr, const QString kernel_name="", const QString connection_file="", bool auto_restart = true);

    ~KernelManager() override;

    void startKernel();
    KernelClient* getNewClient();
private:
    QString kernel_name_{};
    QString connection_file_{};
    bool is_auto_restart_{true};
    bool is_restarting_{false};

    void startRestarter();
    void stopRestarter();
    void postRestarter();
    void handleKernelRestarting();
};


#endif //SCLAB_KERNEL_MANAGER_H
