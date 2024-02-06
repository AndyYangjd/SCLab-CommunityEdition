#ifndef SCLAB_KERNEL_CLIENT_H
#define SCLAB_KERNEL_CLIENT_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class KernelClient; }
QT_END_NAMESPACE

class KernelClient : public QWidget {
Q_OBJECT

public:
    explicit KernelClient(QWidget *parent = nullptr);

    ~KernelClient() override;

    void startChannels();
private:

};


#endif //SCLAB_KERNEL_CLIENT_H
