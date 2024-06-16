#include "bishop.h"

Bishop::Bishop(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::Bishop;
}

QList<Position> Bishop::getAttackMove() {
    QList<Position> l;
    for (int x = pos.x - 1, y = pos.y - 1; x >= 1 and y >= 1; x--, y--) {
        l.append(Position{x, y});
    }
    for (int x = pos.x - 1, y = pos.y + 1; x >= 1 and y <= 8; x--, y++) {
        l.append(Position{x, y});
    }
    for (int x = pos.x + 1, y = pos.y + 1; x <= 8 and y <= 8; x++, y++) {
        l.append(Position{x, y});
    }
    for (int x = pos.x + 1, y = pos.y - 1; x <= 8 and y >= 1; x++, y--) {
        l.append(Position{x, y});
    }

    return l;
}
