#ifndef SCLAB_KERNEL_SELECTION_H
#define SCLAB_KERNEL_SELECTION_H

#include <QDialog>
#include "ui_kernel_selection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class KernelSelection; }
QT_END_NAMESPACE

class KernelSelection : public QDialog {
Q_OBJECT

public:
    explicit KernelSelection(QWidget *parent = nullptr);

    ~KernelSelection() override;

    QString re_kernel_name_{"julia"};
private:
    Ui::KernelSelection *ui_;
private slots:
    void getSelectedKernelName(QAbstractButton *);
};


#endif //SCLAB_KERNEL_SELECTION_H
