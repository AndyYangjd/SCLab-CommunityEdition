#ifndef SCLAB_WELCOME_HOME_H
#define SCLAB_WELCOME_HOME_H

#include <QWidget>
#include "ui_welcome_home.h"


QT_BEGIN_NAMESPACE
namespace Ui { class WelcomeHome; }
QT_END_NAMESPACE

class WelcomeHome : public QWidget {
Q_OBJECT

public:
    explicit WelcomeHome(QWidget *parent = nullptr);

    ~WelcomeHome() override;

private:
    Ui::WelcomeHome *ui;
};


#endif //SCLAB_WELCOME_HOME_H
