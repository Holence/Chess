#include "peerwindow.h"
#include "network/client.h"
#include "network/server.h"
#include "widget/timer.h"
#include <QComboBox>
#include <QDialog>
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRandomGenerator>

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

    if (!connectDialog()) {
        close();
        return;
    }

    connect(peer, &Peer::socketClosed, this, &PeerWindow::socketClosedSlot);
    connect(peer, &Peer::receivedMovement, this, &PeerWindow::receivedMovementSlot);
    connect(peer, &Peer::receivedResign, this, &PeerWindow::receivedResignSlot);

    ////////////////////////////////////////////////////
    // Head Info (timer - name - timer)
    QWidget *head_info = new QWidget();
    head_info->setStyleSheet("background-color: rgb(96, 67, 44); font-size: 12px; color: rgb(255, 255, 255);");
    QHBoxLayout *headLayout = new QHBoxLayout();
    headLayout->setSpacing(0);
    headLayout->setMargin(0);

    QLabel *label_name = new QLabel();
    label_name->setAlignment(Qt::AlignCenter);
    connect(peer, &Peer::receivedOppNickname, this, [this, label_name](QString oppName) {
        label_name->setText(QString("%1  vs  %2").arg(peer->getNickname(), oppName));
    });

    Timer *selfTimer = new Timer(3, 0);
    QSpacerItem *timerSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *timerSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    Timer *oppTimer = new Timer(3, 0);
    headLayout->addWidget(selfTimer);
    headLayout->addItem(timerSpacer1);
    headLayout->addWidget(label_name);
    headLayout->addItem(timerSpacer2);
    headLayout->addWidget(oppTimer);

    head_info->setLayout(headLayout);
    ui->centerLayout->addWidget(head_info);

    ////////////////////////////////////////////////////
    // Board
    selfColor = peer->getSelfColor();
    board = new Board(this, selfColor, isPlayingMode);
    connect(board, &Board::pieceMoved, this, &PeerWindow::pieceMovedSlot);
    bondBoardSlot();
    ui->centerLayout->addWidget(board);

    currentColor = Piece_Color::White;
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);

    // 另外绑定到通知对方
    disconnect(actionResign, nullptr, nullptr, nullptr);
    connect(actionResign, &QAction::triggered, peer, [this] {
        // 让先通信
        peer->sendResign();
        // 再gameEndSlot
        if (selfColor == Piece_Color::White)
            return BaseMainWindow::gameEndSlot(GameState::BlackWin);
        else
            return BaseMainWindow::gameEndSlot(GameState::WhiteWin);
    });

    ////////////////////////////////////////////////////
    // Status Bar
    QWidget *statusBar = new QWidget();
    statusBar->setStyleSheet("background-color: rgb(96, 67, 44);font-size: 12px;");

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
    connect(peer, &Peer::receivedTime, this, [label_latency](int latency) {
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
    connect(peer, &Peer::socketError, chatbox, &ChatBox::receivedMessage); // 其他的错误信息就写在chatbox中
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

bool PeerWindow::connectDialog() {
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
    replay->addMovement(m);
    peer->sendMovement(m);
    currentColor = flipPieceColor(currentColor);
    // 更新是否允许点击事件
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);
}

void PeerWindow::receivedMovementSlot(Movement m) {
    replay->addMovement(m);
    board->movePiece(m);
    currentColor = flipPieceColor(currentColor);
    // 更新是否允许点击事件
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);
}

void PeerWindow::receivedResignSlot() {
    BaseMainWindow::gameEndSlot(GameState::Unfinished);
}

void PeerWindow::socketClosedSlot() {
    if (board->isEnabled()) {
        QMessageBox::critical(this, "Critial", "Socket closed accidentally.\n(perhaps your opponent doesn't want to play with you😥)");
    } else {
        QMessageBox::information(this, "Information", "Your opponent left the game.\n Window will close now.");
        close();
    }
}
