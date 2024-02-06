#ifndef SCLAB_STATUSBAR_H
#define SCLAB_STATUSBAR_H

#include <QWidget>
#include <QDebug>
#include "ui_status_bar.h"


QT_BEGIN_NAMESPACE
namespace Ui { class StatusBar; }
QT_END_NAMESPACE

class StatusBar : public QWidget {
Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);

    ~StatusBar() override;

private:
    Ui::StatusBar *ui_;

signals:

private slots:
    void updateCursorPos(int id_row, int id_col);
};


#endif //SCLAB_STATUSBAR_H
