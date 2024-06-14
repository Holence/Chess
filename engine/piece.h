#ifndef PIECE_H
#define PIECE_H

#include "position.h"
#include <QList>

class Piece {
public:
    Piece();
    QList<Position> getPossibleMove(Piece **board, Position pos);
};

#endif // PIECE_H
