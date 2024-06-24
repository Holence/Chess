#include "mainwindow.h"
#include "peerwindow.h"
#include "replaywindow.h"
#include "singleplayerwindow.h"
#include <QFileDialog>
#include <QMessageBox>
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
        (new SinglePlayerWindow(this))->show();
    });
    layout->addWidget(b1);

    QPushButton *b2 = new QPushButton("Replay Mode");
    connect(b2, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName(this, "Open Replay File", "./replay", "Replay (*.rep)");
        if (!filename.isEmpty()) {
            (new ReplayWindow(this, filename))->show();
        }
    });
    layout->addWidget(b2);

    QPushButton *b3 = new QPushButton("Host Server");
    connect(b3, &QPushButton::clicked, this, [this] {
        new PeerWindow(this, true);
    });
    layout->addWidget(b3);

    QPushButton *b4 = new QPushButton("Join Server");
    connect(b4, &QPushButton::clicked, this, [this] {
        new PeerWindow(this, false);
    });
    layout->addWidget(b4);

    QPushButton *b5 = new QPushButton("About");
    connect(b5, &QPushButton::clicked, this, [this] {
        QMessageBox msg(this);
        msg.setWindowTitle("About");
        msg.setTextFormat(Qt::RichText);
        msg.setIconPixmap(QPixmap(":/img/wn.png").scaled(64, 64));
        msg.setText("Author: Holence<br><br>Github: <a href='https://github.com/Holence/Chess/'>Repo</a>");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    });
    layout->addWidget(b5);

    centralwidget->setLayout(layout);
    setCentralWidget(centralwidget);
}
