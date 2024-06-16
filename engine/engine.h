#ifndef ENGINE_H
#define ENGINE_H

#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *

enum class GameState {
    Win,
    Lose,
    Draw,
    Unfinished
};

class Engine {
public:
    Engine();
    ~Engine();

    GameState nextGameState(Position from, Position to);

    QList<Position> getPossibleMove(Position pos);
    Piece *getPiece(Position pos);

    void newGame(Piece_Color selfColor);
    void endGame();

    GameState getGameState();

private:
    Piece_Color selfColor;
    GameState state;
    GameState checkGameState();

    Cell *board; // board是指向Cell array的指针
    QList<Piece *> SelfPieces;
    QList<Piece *> SelfDeadPieces;
    QList<Piece *> OppPieces;
    QList<Piece *> OppDeadPieces;

    Piece *SelfKing;
    Piece *OppKing;

    void putPiece(Piece *p, Position pos);
    void movePiece(Position from, Position to);

    int caclPressure(Position pos, Piece_Color color);
    bool hasPressure(Position pos, Piece_Color color);

    QList<Position> getBasicFilteredPos(Piece *p);
    QList<Position> getStrictFilteredPos(Piece *p);
};

#endif // ENGINE_H
