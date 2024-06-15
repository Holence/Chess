#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {

public:
    King(Piece_Type type, Piece_Color color, Position pos);
    QList<Position> getPossibleMove();
};

#endif // KING_H
