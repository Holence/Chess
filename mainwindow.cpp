#include "mainwindow.h"
#include "replaywindow.h"
#include "singleplayerwindow.h"
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent} {
    setWindowTitle("Chess");
    setMinimumWidth(250);

    QWidget *centralwidget = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();
    QPushButton *b1 = new QPushButton("Single Player Mode");
    connect(b1, &QPushButton::clicked, this, [this] {
        SinglePlayerWindow *w = new SinglePlayerWindow(this);
        w->show();
    });
    layout->addWidget(b1);

    QPushButton *b2 = new QPushButton("Replay Mode");
    connect(b2, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName(this, "Open Replay File", "./replay", "Replay (*.rep)");
        if (!filename.isEmpty()) {
            ReplayWindow *w = new ReplayWindow(this, filename);
            w->show();
        }
    });
    layout->addWidget(b2);

    centralwidget->setLayout(layout);
    setCentralWidget(centralwidget);
}
