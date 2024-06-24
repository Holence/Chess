#include "singleplayerwindow.h"

SinglePlayerWindow::SinglePlayerWindow(QWidget *parent) : BaseMainWindow(parent, true) {
    setWindowTitle("Single Player Mode");
    setFixedSize(800, 846);
    ui->statusBar->hide();

    selfColor = Piece_Color::White;

    board = new Board(this, selfColor, isPlayingMode);
    connect(board, &Board::pieceMoved, this, &SinglePlayerWindow::pieceMovedSlot);
    bondBoardSlot();
    ui->centerLayout->addWidget(board);

    // Replay
    replay = new Replay(this, selfColor);
}

void SinglePlayerWindow::pieceMovedSlot(Movement m) {
    // 单人游戏 移动棋子后更换selfColor
    board->flipSelfColor();
    replay->addMovement(m);
}
