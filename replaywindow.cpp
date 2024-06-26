#include "replaywindow.h"
#include "replay.h"

ReplayWindow::ReplayWindow(QWidget *parent, QString &filename) : BaseMainWindow(parent, false) {
    setWindowTitle("Replay Mode");
    setFixedSize(800, 846);
    ui->statusBar->hide();

    Replay replay = Replay(this, filename);
    selfColor = replay.getSelfColor();

    board = new Board(this, selfColor, isPlayingMode);
    bondBoardSlot();
    ui->centerLayout->addWidget(board);

    actionNext_Move.setText("Next Move");
    actionNext_Move.setShortcut(tr("Space"));
    ui->menuGame->addAction(&actionNext_Move);
    connect(&actionNext_Move, &QAction::triggered, this, &ReplayWindow::ReplayNextMove);

    movementList = replay.getMovementList();
    replayIndex = -1;
}

void ReplayWindow::ReplayNextMove() {
    if (replayIndex < movementList.length() - 1) {
        replayIndex++;
        board->movePiece(movementList.at(replayIndex));
    }
}

// 如果只是用Movement与engine.DeadPiece的信息去做回溯，感觉太麻烦了……
// 如果replay里再加上eatenPiece的信息，应该就能动态回溯了
// 懒得做了
// TODO ReplayPreviousMove
