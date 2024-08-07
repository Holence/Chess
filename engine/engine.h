#ifndef ENGINE_H
#define ENGINE_H

#include "pawn.h"
#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *

enum class GameState {
    WhiteWin,
    BlackWin,
    Draw,
    Unfinished
};

class Engine {
public:
    Engine();
    ~Engine();

    Piece *movePiece(Movement m);
    GameState checkGameState(Piece_Color color);

    QList<Position> getPossibleMove(Position pos);
    QList<Position> getVisibleArea(Piece_Color color);
    Piece *getPiece(Position pos);

    bool isBeingCheckmated(Piece_Color color);
    bool isKingAlive(Piece_Color color);

private:
    Cell *board = nullptr; // board是指向Cell array的指针
    QList<Piece *> WhitePieces;
    QList<Piece *> WhiteDeadPieces;
    QList<Piece *> BlackPieces;
    QList<Piece *> BlackDeadPieces;

    Piece *WhiteKing = nullptr;
    Piece *BlackKing = nullptr;

    void placePiece(Piece_Color color, bool reversed);
    void putPiece(Piece *p, Position pos);
    void clearPos(Position pos);

    int caclPressure(Position pos, Piece_Color color);
    bool hasPressure(Position pos, Piece_Color color);

    QList<Position> getAttackingPos(Piece *p);
    QList<Position> getMovablePos(Piece *p);

    Pawn *EnPassantPawn = nullptr; // 之前一个State时（对手）前进两格的Pawn的Position

    bool checkOtherDraw();
};

#endif // ENGINE_H
