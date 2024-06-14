#ifndef ENGINE_H
#define ENGINE_H

#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *
// Board是Cell的Array
#define Board Cell *

enum class GameState {
    WhiteWin,
    BlackWin,
    Draw,
    Unfinished,
    WhiteCheckmated,
    BlackCheckmated,
};

class Engine {
public:
    Engine();
    ~Engine();
    Piece *getPiece(Position pos);
    QList<Position> getPossibleMove(Position pos);

private:
    Board board;
    GameState state;
    Position WhiteKingPos;
    Position BlackKingPos;
};

#endif // ENGINE_H
