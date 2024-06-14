#include "king.h"

King::King(Piece_Type type, Piece_Color color) : Piece(type, color) {}

QList<Position> King::getPossibleMove(Position pos) {
    QList<Position> l;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 and j != 0) {
                Position p{pos.x + i, pos.y + j};
                if (isValidPos(p))
                    l.append(p);
            }
        }
    };
    return l;
}
