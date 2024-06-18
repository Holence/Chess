#include "knight.h"

Knight::Knight(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::knight;
}

QList<Position> Knight::getAttackMove() {
    QList<Position> l;
    int m[] = {-2, -1, 1, 2};
    for (int i : m) {
        for (int j : m) {
            if (std::abs(i) + std::abs(j) == 3) {
                Position p{pos.x + i, pos.y + j};
                if (p.isValid())
                    l.append(p);
            }
        }
    }

    return l;
}
