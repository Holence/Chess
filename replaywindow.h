#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include "basemainwindow.h"
#include "board.h"
#include "replay.h"

class ReplayWindow : public BaseMainWindow {
public:
    ReplayWindow(QWidget *parent, QString &filename);
    ~ReplayWindow();

private:
    Piece_Color selfColor;

    Board *board;
    QList<Replay::Movement> movementList;

    int replayIndex;
    void ReplayNextMove();

    QAction actionNext_Move;
};

#endif // REPLAYWINDOW_H
