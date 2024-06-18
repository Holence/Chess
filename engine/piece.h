#ifndef PIECE_H
#define PIECE_H

#include "position.h"
#include <QList>

enum Piece_Type : char { pawn = 'p',
                         king = 'k',
                         queen = 'q',
                         rook = 'r',
                         bishop = 'b',
                         knight = 'n',
                         null = '0' };

enum Piece_Color : char { White = 'w',
                          Black = 'b' };

class Piece {
public:
    Piece(Piece_Color color, Position pos);
    virtual QList<Position> getAttackMove() = 0;
    virtual ~Piece() = default;
    Piece_Color getColor();
    Piece_Type getType();
    Position getPos();

    void setType(Piece_Type type);
    void setColor(Piece_Color newColor);
    void setPos(Position pos);

protected:
    Piece_Type type;
    Piece_Color color;
    Position pos;
};

Piece_Color flipPieceColor(Piece_Color color);

#endif // PIECE_H
