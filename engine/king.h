#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {

public:
    King(Piece_Type type, Piece_Color color);
    QList<Position> getPossibleMove(Position pos);
};

#endif // KING_H
