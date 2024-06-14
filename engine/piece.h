#ifndef PIECE_H
#define PIECE_H

#include "position.h"
#include <QList>

enum class Piece_Type { Pawn, King, Queen, Rook, Bishop, Knight };
enum class Piece_Color { White, Black };

class Piece {
public:
    Piece(Piece_Type type, Piece_Color color);
    virtual QList<Position> getPossibleMove(Position pos) = 0;
    virtual ~Piece() = default;
    Piece_Color getColor();

private:
    Piece_Type type;
    Piece_Color color;
};

#endif // PIECE_H
