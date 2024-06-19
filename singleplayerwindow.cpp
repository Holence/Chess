#include "singleplayerwindow.h"
#include <QMessageBox>

SinglePlayerWindow::SinglePlayerWindow(QWidget *parent) : BaseMainWindow(parent) {

    // TODO select color box
    selfColor = Piece_Color::White;

    actionResign.setText("Resign");
    ui->menuGame->addAction(&actionResign);
    connect(&actionResign, &QAction::triggered, this, [this] {
        if (selfColor == Piece_Color::White)
            return gameEndSlot(GameState::BlackWin);
        else
            return gameEndSlot(GameState::WhiteWin);
    });

    board = new Board(this, selfColor, false);
    // 单人游戏 移动棋子后更换selfColor
    connect(board, &Board::pieceMoved, this, &SinglePlayerWindow::pieceMovedSlot);
    connect(board, &Board::gameEnded, this, &SinglePlayerWindow::gameEndSlot);
    ui->centerLayout->addWidget(board);

    // Replay
    replay = new Replay(selfColor);
}

SinglePlayerWindow::~SinglePlayerWindow() {
    // 如果玩到一半就退了，那也保存一下replay
    if (replay) {
        replay->replaySave();
        delete replay;
    }
    delete board;
}

void SinglePlayerWindow::pieceMovedSlot(Position pos_from, Position pos_to, Piece_Type promoteType) {
    board->flipSelfColor();
    replay->addMovement(pos_from, pos_to, promoteType);
}

void SinglePlayerWindow::gameEndSlot(GameState state) {
    // 记录结束，保存记录
    replay->replaySave();
    delete replay;
    replay = nullptr;

    QString s;

    switch (state) {
    case GameState::WhiteWin:
        if (selfColor == Piece_Color::White)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::BlackWin:
        if (selfColor == Piece_Color::Black)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::Draw:
        s = QString("Draw");
        break;
    default:
        // Resign
        s = QString("Resigned");
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Game Over");
    msg.setText(s);
    msg.adjustSize();
    msg.exec();

    actionResign.setDisabled(true);
    board->setDisabled(true);
}
