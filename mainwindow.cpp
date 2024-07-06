#include "mainwindow.h"
#include "peerwindow.h"
#include "replaywindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#ifndef RTS_MODE
#include "singleplayerwindow.h"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent} {

#ifndef RTS_MODE
    setWindowTitle("Chess");
#else
    setWindowTitle("Chess [RTS]");
#endif

    setMinimumWidth(250);

    QWidget *centralwidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

#ifndef RTS_MODE
    QPushButton *bs = new QPushButton("Single Player Mode");
    connect(bs, &QPushButton::clicked, this, [this] {
        (new SinglePlayerWindow(this))->show();
    });
    layout->addWidget(bs);
#endif

    QPushButton *bh = new QPushButton("Host Server");
    connect(bh, &QPushButton::clicked, this, [this] {
        new PeerWindow(this, true);
    });
    layout->addWidget(bh);

    QPushButton *bj = new QPushButton("Join Server");
    connect(bj, &QPushButton::clicked, this, [this] {
        new PeerWindow(this, false);
    });
    layout->addWidget(bj);

    QPushButton *br = new QPushButton("Replay Mode");
    connect(br, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName(this, "Open Replay File", "./replay", "Replay (*.rep)");
        if (!filename.isEmpty()) {
            (new ReplayWindow(this, filename))->show();
        }
    });
    layout->addWidget(br);

    QPushButton *ba = new QPushButton("About");
    connect(ba, &QPushButton::clicked, this, [this] {
        QMessageBox msg(this);
        msg.setWindowTitle("About");
        msg.setTextFormat(Qt::RichText);
        msg.setIconPixmap(QPixmap(":/img/wn.png").scaled(64, 64));
        msg.setText("Version: 1.1.0.0<br><br>Author: Holence<br><br>Github: <a href='https://github.com/Holence/Chess/'>Repo</a>");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    });
    layout->addWidget(ba);

    centralwidget->setLayout(layout);
    setCentralWidget(centralwidget);
}
