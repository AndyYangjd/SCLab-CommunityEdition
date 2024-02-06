#ifndef SCLAB_PREFERENCES_H
#define SCLAB_PREFERENCES_H

#include <qDebug>
#include <QDialog>
#include <QAction>
#include "ui_preferences.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Preferences; }
QT_END_NAMESPACE

class Preferences : public QDialog {
Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);

    ~Preferences() override;

private:
    Ui::Preferences *ui_;

    void setSelfDefinedUI();
    void setConnections();

signals:

private slots:
    void onTreeWidgetItemClicked(QTreeWidgetItem *, int);
};


#endif //SCLAB_PREFERENCES_H
