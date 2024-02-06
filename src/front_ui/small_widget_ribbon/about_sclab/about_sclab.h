#ifndef SCLAB_ABOUT_SCLAB_H
#define SCLAB_ABOUT_SCLAB_H

#include <QDialog>
#include "ui_about_sclab.h"


QT_BEGIN_NAMESPACE
namespace Ui { class AboutSClab; }
QT_END_NAMESPACE

class AboutSClab : public QDialog {
Q_OBJECT

public:
    explicit AboutSClab(QWidget *parent = nullptr);

    ~AboutSClab() override;
    QString getStatement();
    void setStatement(const QString& str);
private:
    Ui::AboutSClab *ui_;

private slots:
};


#endif //SCLAB_ABOUT_SCLAB_H
