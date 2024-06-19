#include "mainwindow.h"
#include <QApplication>

// TODO Online Mode
// TODO Chat
// TODO Taunt
// TODO Sound/BGM
// TODO Memory Leak Check
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
