#include "kernel_manager.h"
#include <qDebug>

KernelManager::KernelManager(QWidget *parent , QString kernel_name, QString connection_file, bool auto_restart) :
        QWidget(parent), kernel_name_(kernel_name), connection_file_(connection_file),is_auto_restart_(auto_restart){
}

KernelManager::~KernelManager() {
}

void KernelManager::startKernel() {

}

KernelClient* KernelManager::getNewClient(){
    auto* new_kernel_client = new KernelClient(this);

    return new_kernel_client;
}

void KernelManager::startRestarter() {

}

void KernelManager::stopRestarter() {
    if (is_auto_restart_){
        ;
    }
}

void KernelManager::postRestarter() {
    if (is_restarting_){
        is_restarting_= false;
    }
}

void KernelManager::handleKernelRestarting() {
    is_restarting_= true;
}