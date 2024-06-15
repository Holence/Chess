#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *

class Board {
public:
    Board();
    ~Board();
    Piece *getPiece(Position pos);
    void putPiece(Piece *p, Position pos);
    void movePiece(Position from, Position to);
    QList<Position> getPossibleMove(Position pos);

private:
    Cell *board; // board是指向Cell array的指针
    QList<Piece *> WhitePieces;
    QList<Piece *> WhiteDeadPieces;
    QList<Piece *> BlackPieces;
    QList<Piece *> BlackDeadPieces;
    Position WhiteKingPos;
    Position BlackKingPos;
};

#endif // BOARD_H
