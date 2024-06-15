#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), engine() {
    ui->setupUi(this);
    CellButton *p;
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            p = new CellButton(Position{x, y});
            ui->gridLayout->addWidget(p, 8 - x, y - 1);
            // 用lambda function可以传参
            // connect(p, p->clicked, this, [this, x, y] { cellClicked(Position{x, y}); });
            connect(p, &CellButton::leftClicked, this, &MainWindow::cellClicked);
            connect(p, &CellButton::rightClicked, this, &MainWindow::cellCanceled);
            cellList.append(p);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
    CellButton *p;
    foreach (p, cellList) {
        delete p;
    }
}

void MainWindow::cellClicked(Position pos) {
    QList<Position> l;
    l = engine.getPossibleMove(pos);
    foreach (Position p, l) {
        qInfo() << p.x << " " << p.y;
    }
}

void MainWindow::cellCanceled(Position pos) {
    qInfo() << "Cancel" << pos.x << " " << pos.y;
}
