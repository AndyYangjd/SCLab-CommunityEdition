#ifndef SCLAB_SAY_THANKS_H
#define SCLAB_SAY_THANKS_H

#include <QDialog>
#include "ui_say_thanks.h"


QT_BEGIN_NAMESPACE
namespace Ui { class SayThanks; }
QT_END_NAMESPACE

class SayThanks : public QDialog {
Q_OBJECT

public:
    explicit SayThanks(QWidget *parent = nullptr);

    ~SayThanks() override;

private:
    Ui::SayThanks *ui;
};


#endif //SCLAB_SAY_THANKS_H
