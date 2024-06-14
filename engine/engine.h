#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"

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
    GameState checkGameState();
    GameState nextGameState(Position from, Position to);
    QList<Position> getPossibleMove(Position pos);

private:
    Board board;
    GameState state;
};

#endif // ENGINE_H
