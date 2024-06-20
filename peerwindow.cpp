#include "peerwindow.h"
#include "network/client.h"
#include "network/server.h"
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

// TODO Initialize Pack: Name PieceColor
// TODO Chat
PeerWindow::PeerWindow(QWidget *parent, bool isServer) : BaseMainWindow(parent, true) {
    setWindowTitle("Online Play Mode");

    this->isServer = isServer;

    if (isServer) {
        selfColor = Piece_Color::White;
    } else {
        selfColor = Piece_Color::Black;
    }

    if (!connectSuccessed()) {
        close();
        return;
    }

    board = new Board(this, selfColor, true);
    connect(board, &Board::pieceMoved, this, &PeerWindow::pieceMovedSlot);
    bondBoardSlot();
    ui->centerLayout->addWidget(board);

    replay = new Replay(selfColor);

    currentColor = Piece_Color::White;
    board->setEnabled(currentColor == selfColor);

    connect(peer, &Peer::receivedMovement, this, &PeerWindow::receivedMovementSlot);
    connect(peer, &Peer::socketError, this, &PeerWindow::socketErrorSlot);

    show();
}

PeerWindow::~PeerWindow() {
    peer->disconnect();
    delete peer;
}

bool PeerWindow::connectSuccessed() {
    QDialog *dlg = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    QVBoxLayout *layout = new QVBoxLayout(dlg);

    if (isServer) {
        // Server
        peer = new Server(this);
        dlg->setWindowTitle("Host Server");

        QLabel *label = new QLabel("Waiting connection...");
        QPushButton *rejectButton = new QPushButton("Cancel");

        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectSuccessed, dlg, [dlg] { dlg->accept(); });

        layout->addWidget(label);
        layout->addWidget(rejectButton);

    } else {
        // Client
        peer = new Client(this);
        dlg->setWindowTitle("Join Server");

        QLineEdit *ip_edit = new QLineEdit("127.0.0.1");
        ip_edit->setPlaceholderText("hostname or ip");
        QPushButton *connectButton = new QPushButton("Connect");
        QPushButton *rejectButton = new QPushButton("Cancel");

        connect(connectButton, &QPushButton::clicked, dlg, [this, ip_edit] {
            QString ip = ip_edit->text();
            ((Client *)peer)->connectToServer(ip, 11451);
        });
        connect(rejectButton, &QPushButton::clicked, dlg, [dlg] { dlg->reject(); });
        connect(peer, &Peer::connectSuccessed, dlg, [dlg] { dlg->accept(); });

        layout->addWidget(ip_edit);
        layout->addWidget(connectButton);
        layout->addWidget(rejectButton);
    }
    dlg->setLayout(layout);

    if (dlg->exec() == QDialog::Rejected) {
        return false;
    } else {
        return true;
    }
}

void PeerWindow::pieceMovedSlot(Movement m) {
    replay->addMovement(m);
    peer->sendMovement(m);
    currentColor = flipPieceColor(currentColor);
    board->setEnabled(currentColor == selfColor);
}

void PeerWindow::receivedMovementSlot(Movement m) {
    replay->addMovement(m);
    board->movePiece(m);
    currentColor = flipPieceColor(currentColor);
    board->setEnabled(currentColor == selfColor);
}

void PeerWindow::socketErrorSlot(QString error) {
    QMessageBox::critical(this, "Critial", error);
    close();
}
