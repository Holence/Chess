#include "replaywindow.h"

ReplayWindow::ReplayWindow(QWidget *parent, QString &filename) : BaseMainWindow(parent) {

    Replay replay = Replay(filename);
    selfColor = replay.getSelfColor();
    board = new Board(this, selfColor, true);
    ui->centerLayout->addWidget(board);

    actionNext_Move.setText("Next Move");
    actionNext_Move.setShortcut(tr("Space"));
    ui->menuGame->addAction(&actionNext_Move);
    connect(&actionNext_Move, &QAction::triggered, this, &ReplayWindow::ReplayNextMove);

    movementList = replay.getMovementList();
    replayIndex = -1;
}

ReplayWindow::~ReplayWindow() {
    delete board;
}

void ReplayWindow::ReplayNextMove() {
    if (replayIndex < movementList.length() - 1) {
        replayIndex++;
        Replay::Movement m = movementList.at(replayIndex);
        board->movePiece(m.pos_from, m.pos_to, m.promteType);
    }
}

// 如果只是用Movement与engine.DeadPiece的信息去做回溯，感觉太麻烦了……
// 懒得做了
// TODO ReplayPreviousMove
