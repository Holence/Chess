#ifndef ENGINE_H
#define ENGINE_H

#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *

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
    Piece *getPiece(Position pos);

private:
    GameState state;

    Cell *board; // board是指向Cell array的指针
    QList<Piece *> WhitePieces;
    QList<Piece *> WhiteDeadPieces;
    QList<Piece *> BlackPieces;
    QList<Piece *> BlackDeadPieces;
    Position WhiteKingPos;
    Position BlackKingPos;

    void putPiece(Piece *p, Position pos);
    void movePiece(Position from, Position to);
};

#endif // ENGINE_H
