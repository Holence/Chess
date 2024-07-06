#include "replaywindow.h"
#include "replay.h"

ReplayWindow::ReplayWindow(QWidget *parent, QString &filename) : BaseMainWindow(parent, false) {
#ifndef RTS_MODE
    setWindowTitle("Replay Mode");
#else
    setWindowTitle("Replay Mode - Self Vision");
#endif
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

#ifdef RTS_MODE
    actionChange_Vision.setText("Change Vision");
    ui->menuGame->addAction(&actionChange_Vision);
    actionChange_Vision.setShortcut(tr("F"));
    connect(&actionChange_Vision, &QAction::triggered, this, [this] {
        vision_mode = (vision_mode + 1) % 3;
        switch (vision_mode) {
        case 0:
            board->setSelfColor(selfColor);
            board->setAllBoardVision(false);
            setWindowTitle("Replay Mode - Self Vision");
            break;
        case 1:
            board->setSelfColor(flipPieceColor(selfColor));
            board->setAllBoardVision(false);
            setWindowTitle("Replay Mode - Opponent Vision");
            break;
        case 2:
            board->setAllBoardVision(true);
            setWindowTitle("Replay Mode - Full Vision");
            break;
        default:
            break;
        }
        board->refreshBoard();
    });
#endif
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
