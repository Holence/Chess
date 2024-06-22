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

PeerWindow::PeerWindow(QWidget *parent, bool isServer) : BaseMainWindow(parent, true) {
    setWindowTitle("Online Play Mode");

    this->isServer = isServer;

    if (!connectDialog()) {
        close();
        return;
    }

    connect(peer, &Peer::socketClosed, this, &PeerWindow::socketClosedSlot);

    selfColor = peer->getSelfColor();
    board = new Board(this, selfColor, true);
    connect(board, &Board::pieceMoved, this, &PeerWindow::pieceMovedSlot);
    bondBoardSlot();
    ui->centerLayout->addWidget(board);

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

    replay = new Replay(selfColor);

    currentColor = Piece_Color::White;
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);

    connect(peer, &Peer::receivedMovement, this, &PeerWindow::receivedMovementSlot);
    connect(peer, &Peer::receivedResign, this, &PeerWindow::receivedResignSlot);

    // Chatbox
    chatbox = new ChatBox("Chat Box", this);
    connect(chatbox, &ChatBox::sendMessage, peer, &Peer::sendMessage);
    connect(peer, &Peer::receivedMessage, chatbox, &ChatBox::receivedMessage);

    // 其他的错误信息就写在chatbox中
    connect(peer, &Peer::socketError, chatbox, &ChatBox::receivedMessage);

    // Taunt
    QList<QString> countryList = {"am", "br", "cu", "fr", "ge", "ir", "ko", "li", "ru", "yu"};
    tauntCountry = countryList[QRandomGenerator::global()->bounded(countryList.length())];
    for (int i = 0; i < 8; i++) {
        QAction *a = new QAction(this);
        a->setShortcut(QKeySequence("F" + QString::number(i + 1)));
        connect(a, &QAction::triggered, this, [this, i] { playTaunt(tauntCountry, i + 1); });
        connect(a, &QAction::triggered, peer, [this, i] { peer->sendTaunt(tauntCountry, i + 1); });
        addAction(a);
    }
    connect(peer, &Peer::receivedTaunt, this, &PeerWindow::playTaunt);

    show();
}

PeerWindow::~PeerWindow() {
    peer->disconnectSocket();
    delete peer;
}

bool PeerWindow::connectDialog() {
    QDialog *dlg = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    QVBoxLayout *layout = new QVBoxLayout(dlg);

    if (isServer) {
        // Server
        peer = new Server(this);
        dlg->setWindowTitle("Host Server");

        QLabel *label = new QLabel("Waiting connection...");
        QPushButton *rejectButton = new QPushButton("Cancel");

        QComboBox *combo = new QComboBox();
        combo->addItem("White");
        combo->addItem("Black");
        peer->setSelfColor(Piece_Color::White);

        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), peer, [this](int index) {
            if (index == 0)
                peer->setSelfColor(Piece_Color::White);
            else
                peer->setSelfColor(Piece_Color::Black);
        });
        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectionSuccessed, dlg, [dlg] { dlg->accept(); });

        layout->addWidget(label);
        layout->addWidget(combo);
        layout->addWidget(rejectButton);

    } else {
        // Client
        peer = new Client(this);
        dlg->setWindowTitle("Join Server");

        QLineEdit *ip_edit = new QLineEdit("127.0.0.1");
        ip_edit->setPlaceholderText("hostname or ip");
        QPushButton *connectButton = new QPushButton("Connect");
        QPushButton *rejectButton = new QPushButton("Cancel");

        connect(connectButton, &QPushButton::clicked, dlg, [this, ip_edit, connectButton] {
            QString ip = ip_edit->text();
            ((Client *)peer)->connectToServer(ip, 11451);
            connectButton->setDisabled(true);
        });
        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectionSuccessed, dlg, [dlg] { dlg->accept(); });
        connect(peer, &Peer::socketError, dlg, [dlg, connectButton](QString error) {
            QMessageBox::critical(dlg, "Critial", error);
            connectButton->setEnabled(true);
        });

        layout->addWidget(ip_edit);
        layout->addWidget(connectButton);
        layout->addWidget(rejectButton);
    }
    dlg->setLayout(layout);

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
    board->setAttribute(Qt::WA_TransparentForMouseEvents, currentColor != selfColor);
}

void PeerWindow::receivedMovementSlot(Movement m) {
    replay->addMovement(m);
    board->movePiece(m);
    currentColor = flipPieceColor(currentColor);
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
