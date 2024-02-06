#include <QApplication>
#include <QPushButton>

#include "main_window/main_window.h"

// #include <signal.h>

// void handleQuitSignal(int sig){
//     if (sig==SIGINT){
//         qApp->quit();
//     }
// }

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFont f = app.font();

    f.setFamily("微软雅黑");
    app.setFont(f);

    MainWindow window{};
    window.show();

    // QObject::connect(&app, SIGNAL(aboutToQuit()), &window, SLOT(shutDownKernelsOnExit()));
    // QObject::connect(&window, SIGNAL(quitWindow()), &app, SLOT(quit()));
    // signal(SIGINT, handleQuitSignal);

    return (app.exec());
}