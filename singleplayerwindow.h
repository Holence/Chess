#ifndef SINGLEPLAYERWINDOW_H
#define SINGLEPLAYERWINDOW_H

#include "board.h"
#include "replay.h"
#include "basemainwindow.h"

class SinglePlayerWindow : public BaseMainWindow {
    Q_OBJECT

public:
    SinglePlayerWindow(QWidget *parent = nullptr);
    ~SinglePlayerWindow();

private slots:
    void pieceMovedSlot(Position pos_from, Position pos_to, Piece_Type promoteType);
    void gameEndSlot(GameState state);

private:
    Piece_Color selfColor;

    Board *board;
    Replay *replay;

    QAction actionResign;
};
#endif // SINGLEPLAYERWINDOW_H
