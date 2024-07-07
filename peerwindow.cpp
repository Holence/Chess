#include "peerwindow.h"
#include "network/client.h"
#include "network/server.h"
#include <QComboBox>
#include <QDialog>
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTimeEdit>

QMap<QString, QString> countryMap = {
    {"America", "am"},
    {"Britain", "br"},
    {"Cuba", "cu"},
    {"France", "fr"},
    {"German", "ge"},
    {"Iraq", "ir"},
    {"Korea", "ko"},
    {"Libya", "li"},
    {"Russia", "ru"},
    {"Yuri", "yu"},
};

PeerWindow::PeerWindow(QWidget *parent, bool isServer) : BaseMainWindow(parent, true) {
#ifndef RTS_MODE
    setWindowTitle("Online Play Mode");
#else
    setWindowTitle("Online Play Mode [RTS]");
#endif
    setFixedSize(800, 882);

    settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    this->isServer = isServer;

    if (!openConnectDialog()) {
        close();
        return;
    }

    // peer emit connectionSuccessed信号，openConnectDialog()返回true，双方已互换颜色
    currentColor = Piece_Color::White;
    selfColor = peer->getSelfColor();

    connect(peer, &Peer::socketClosed, this, &PeerWindow::socketClosedSlot);
    connect(peer, &Peer::receivedMovement, this, &PeerWindow::receivedMovementSlot);
    connect(peer, &Peer::receivedForceEnd, this, &PeerWindow::receivedForceEnd);

    auto forceEndSlot = [this](int reason) {
        // 让先通信
        peer->sendForceLose(reason);
        // 再gameEndSlot
        return PeerWindow::gameForceEndSlot(reason);
    };

    ////////////////////////////////////////////////////
    // Head Info (timer - name - timer)
    QWidget *head_info = new QWidget();
    head_info->setStyleSheet("background-color: rgb(96, 67, 44); font-size: 12px; color: rgb(255, 255, 255);");
    QHBoxLayout *headLayout = new QHBoxLayout();
    headLayout->setSpacing(0);
    headLayout->setMargin(0);

    QLabel *label_name = new QLabel();
    connect(peer, &Peer::receivedOppNickname, this, [this, label_name](QString oppName) {
        label_name->setText(QString("%1  vs  %2").arg(peer->getNickname(), oppName));
    });

    selfTimer = new Timer(peer->getTotalTime());
    connect(selfTimer, &Timer::timeup, this, [forceEndSlot] { forceEndSlot(-2); }); // 自己时间到了
    QSpacerItem *timerSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *timerSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    oppTimer = new Timer(peer->getTotalTime());
    headLayout->addWidget(selfTimer);
    headLayout->addItem(timerSpacer1);
    headLayout->addWidget(label_name);
    headLayout->addItem(timerSpacer2);
    headLayout->addWidget(oppTimer);

    head_info->setLayout(headLayout);
    ui->centerLayout->addWidget(head_info);

    if (currentColor == selfColor) {
        selfTimer->resume();
    } else {
        oppTimer->resume();
    }

    connect(peer, &Peer::receivedSyncTime, oppTimer, &Timer::setTime); // 收到对方的时间同步包

    ////////////////////////////////////////////////////
    // Board
    board = new Board(this, selfColor, isPlayingMode);
    connect(board, &Board::pieceMoved, this, &PeerWindow::pieceMovedSlot);
    bondBoardSlot();

    ui->centerLayout->addWidget(board);

    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);

    // 另外绑定到通知对方
    disconnect(actionResign, nullptr, nullptr, nullptr);
    connect(actionResign, &QAction::triggered, this, [forceEndSlot] { forceEndSlot(-1); }); // 自己投降

    ////////////////////////////////////////////////////
    // Status Bar
    QWidget *statusBar = new QWidget();
    statusBar->setStyleSheet("background-color: rgb(96, 67, 44); font-size: 12px;");

    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->setSpacing(0);
    statusLayout->setMargin(0);

    QLabel *label_info = new QLabel();
    label_info->setStyleSheet("color: rgb(255, 255, 255);");
    connect(peer, &Peer::receivedOppNickname, this, [this, label_info] {
        label_info->setText(QString("%1  |  Latency:  ").arg(peer->getConnectionInfo()));
    });
    statusLayout->addWidget(label_info);

    QLabel *label_latency = new QLabel(statusBar);
    connect(peer, &Peer::receivedPing, this, [label_latency](int latency) {
        label_latency->setText(QString::number(latency) + "ms");
        if (latency < 100) {
            label_latency->setStyleSheet("color: rgb(0,255,0)");
        } else if (latency < 200) {
            label_latency->setStyleSheet("color: rgb(255,255,0)");
        } else {
            label_latency->setStyleSheet("color: rgb(255,0,0)");
        }
    });
    label_latency->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statusLayout->addWidget(label_latency);

    statusBar->setLayout(statusLayout);
    ui->centralwidget->layout()->addWidget(statusBar);

    ////////////////////////////////////////////////////
    // Replay
    replay = new Replay(this, selfColor);
    replay->setSelfName(peer->getNickname());
    connect(peer, &Peer::receivedOppNickname, replay, &Replay::setOppName);

    ////////////////////////////////////////////////////
    // Chatbox
    chatbox = new ChatBox("Chat Box", this);
    chatbox->setSelfName(peer->getNickname());
    connect(peer, &Peer::receivedOppNickname, chatbox, &ChatBox::setOppName);
    connect(chatbox, &ChatBox::sendMessage, peer, &Peer::sendMessage);
    connect(peer, &Peer::receivedMessage, chatbox, &ChatBox::receivedMessage);
    connect(peer, &Peer::socketError, chatbox, &ChatBox::appendText); // 其他的错误信息就写在chatbox中
    chatbox->move(x() + width(), y());
    chatbox->resize(400, height());

    ////////////////////////////////////////////////////
    // Taunt
    for (int i = 0; i < 8; i++) {
        QAction *a = new QAction(this);
        a->setShortcut(QKeySequence("F" + QString::number(i + 1)));
        connect(a, &QAction::triggered, this, [this, i] { playTaunt(tauntCountry, i + 1); });
        connect(a, &QAction::triggered, peer, [this, i] { peer->sendTaunt(tauntCountry, i + 1); });
        addAction(a);
    }
    connect(peer, &Peer::receivedTaunt, this, &PeerWindow::playTaunt);

    ////////////////////////////////////////////////////
    // 等界面、Replay、ChatBox都初始化后，才可以发送Ping、NickName的数据包
    peer->sendReadyToReceiveSignal();
    show();
}

PeerWindow::~PeerWindow() {
    peer->disconnectSocket();
    delete peer;
}

bool PeerWindow::openConnectDialog() {
    if (isServer) {
        peer = new Server(this);
    } else {
        peer = new Client(this);
    }

    QDialog *dlg = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    QVBoxLayout *layout = new QVBoxLayout(dlg);

    QLabel *name_label = new QLabel("Your Nickname");
    QLineEdit *name_edit = new QLineEdit(settings->value("Nickname", "Player").toString());
    peer->setNickname(name_edit->text());
    connect(name_edit, &QLineEdit::textChanged, dlg, [this](QString text) {
        settings->setValue("Nickname", text);
        peer->setNickname(text);
    });

    QLabel *country_label = new QLabel("Your Country");
    QComboBox *country_combo = new QComboBox();
    country_combo->addItems(countryMap.keys());
    tauntCountry = countryMap["America"];
    connect(country_combo, &QComboBox::currentTextChanged, this, [this](QString text) { tauntCountry = countryMap[text]; });

    QFrame *hline = new QFrame();
    hline->setFrameShape(QFrame::HLine);

    if (isServer) {
        // Server
        dlg->setWindowTitle("Host Server");

        QLabel *total_time_label = new QLabel("Timer");
        QTimeEdit *total_time_edit = new QTimeEdit();
        total_time_edit->setTime(QTime(0, 5, 0));
        total_time_edit->setMinimumTime(QTime(0, 0, 1));
        total_time_edit->setMaximumTime(QTime(1, 0, 0));
        total_time_edit->setDisplayFormat("mm : ss");
        peer->setTotalTime(total_time_edit->time());
        connect(total_time_edit, &QTimeEdit::timeChanged, peer, [this](QTime total_time) {
            peer->setTotalTime(total_time);
        });

        QLabel *label = new QLabel("Waiting connection...");
        QPushButton *rejectButton = new QPushButton("Cancel");
        QLabel *port_label = new QLabel("Port: " + QString::number(peer->getPort()));
        port_label->setTextInteractionFlags(Qt::TextSelectableByMouse);

        QLabel *color_label = new QLabel("Your color");
        QComboBox *color_combo = new QComboBox();
        color_combo->addItem("White");
        color_combo->addItem("Black");
        peer->setSelfColor(Piece_Color::White);
        connect(color_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), peer, [this](int index) {
            if (index == 0)
                peer->setSelfColor(Piece_Color::White);
            else
                peer->setSelfColor(Piece_Color::Black);
        });

        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectionSuccessed, dlg, [dlg] { dlg->accept(); });

        layout->addWidget(label);
        layout->addWidget(port_label);

        layout->addWidget(hline);

        layout->addWidget(name_label);
        layout->addWidget(name_edit);
        layout->addWidget(country_label);
        layout->addWidget(country_combo);
        layout->addWidget(color_label);
        layout->addWidget(color_combo);
        layout->addWidget(total_time_label);
        layout->addWidget(total_time_edit);

        layout->addWidget(rejectButton);

    } else {
        // Client
        dlg->setWindowTitle("Join Server");

        QLabel *ip_label = new QLabel("Server Hostname / IP");
        QLineEdit *ip_edit = new QLineEdit("127.0.0.1");
        ip_edit->setPlaceholderText("hostname or ip");

        QLabel *port_label = new QLabel("Server Port");
#ifndef RTS_MODE
        QLineEdit *port_edit = new QLineEdit("11451");
#else
        QLineEdit *port_edit = new QLineEdit("11452");
#endif
        port_edit->setPlaceholderText("port");

        QPushButton *connectButton = new QPushButton("Connect");
        QPushButton *rejectButton = new QPushButton("Cancel");

        connect(connectButton, &QPushButton::clicked, dlg, [this, ip_edit, port_edit, connectButton] {
            QString ip = ip_edit->text();
            int port = port_edit->text().toInt();
            ((Client *)peer)->connectToServer(ip, port);
            connectButton->setDisabled(true);
        });
        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectionSuccessed, dlg, [dlg] { dlg->accept(); });
        connect(peer, &Peer::socketError, dlg, [dlg, connectButton](QString error) {
            QMessageBox::critical(dlg, "Critial", error);
            connectButton->setEnabled(true);
        });

        layout->addWidget(ip_label);
        layout->addWidget(ip_edit);
        layout->addWidget(port_label);
        layout->addWidget(port_edit);

        layout->addWidget(hline);

        layout->addWidget(name_label);
        layout->addWidget(name_edit);
        layout->addWidget(country_label);
        layout->addWidget(country_combo);

        layout->addWidget(connectButton);
        layout->addWidget(rejectButton);
    }
    dlg->setLayout(layout);

    // move to center of parent
    // need to force adjustSize() before show(), or it will get the wrong size()
    dlg->adjustSize();
    dlg->move(parentWidget()->pos() + parentWidget()->rect().center() - dlg->rect().center());

    if (dlg->exec() == QDialog::Rejected) {
        delete dlg;
        return false;
    } else {
        delete dlg;
        return true;
    }
}

void PeerWindow::playTaunt(QString country, int i) {
    board->playMedia("qrc:/taunt/" + country + "0" + QString::number(i) + ".wav");
}

void PeerWindow::pieceMovedSlot(Movement m) {
    selfTimer->stop();
    oppTimer->resume();
    // 如果latency很大，本地显示的对方时钟将会比实际对方的真实时钟快，导致本地显示对方时钟已经到0时，其实对方的时钟还没到0
    // 所以每次发送Movement包的时候，也发送自己的剩余时间，让对方同步一下
    peer->sendSyncTime(selfTimer->getTime());

    replay->addMovement(m);
    peer->sendMovement(m);
    currentColor = flipPieceColor(currentColor);
    // 更新是否允许点击事件
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);
}

void PeerWindow::receivedMovementSlot(Movement m) {
    oppTimer->stop();
    selfTimer->resume();

    replay->addMovement(m);
    board->movePiece(m);
    currentColor = flipPieceColor(currentColor);
    // 更新是否允许点击事件
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);
}

void PeerWindow::receivedForceEnd(int reason) {
    return PeerWindow::gameForceEndSlot(-reason);
}

void PeerWindow::gameForceEndSlot(int reason) {
    if (isPlayingMode) {
        // 保存记录
        replay->replaySave();
        delete replay;
        replay = nullptr;
        actionResign->setDisabled(true);
    }

    board->playMedia("qrc:/sound/game-end.mp3");

    QString s;

    switch (reason) {
    case -1:
        s = QString("You Resigned");
        break;
    case 1:
        s = QString("Opponent Resigned");
        break;
    case -2:
        s = QString("Your time is up.\nYou Lose");
        break;
    case 2:
        s = QString("Opponent's time is up.\nYou Win");
        break;
    default:
        break;
    }

    board->setDisabled(true);

    QMessageBox msg(this);
    msg.setWindowTitle("Game Over");
    msg.setText(s);
    msg.adjustSize();
    msg.exec();
}

void PeerWindow::socketClosedSlot() {
    if (board->isEnabled()) {
        QMessageBox::critical(this, "Critial", "Socket closed accidentally.\n(perhaps your opponent doesn't want to play with you😥)");
    } else {
        chatbox->appendText("-----------------------------------------------------\nYour opponent left the game.");
    }
    chatbox->setDisabled(true);
}
