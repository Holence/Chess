#ifndef SINGLEPLAYERWINDOW_H
#define SINGLEPLAYERWINDOW_H

#include "replay.h"
#include "widget/basemainwindow.h"

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

    Replay *replay;

    QAction actionResign;
};
#endif // SINGLEPLAYERWINDOW_H
