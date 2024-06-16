#include "rook.h"

Rook::Rook(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::Rook;
}

QList<Position> Rook::getPossibleMove() {
    QList<Position> l;
    for (int x = pos.x - 1; x >= 1; x--) {
        Position p{x, pos.y};
        l.append(p);
    }
    for (int x = pos.x + 1; x <= 8; x++) {
        Position p{x, pos.y};
        l.append(p);
    }
    for (int y = pos.y - 1; y >= 1; y--) {
        Position p{pos.x, y};
        l.append(p);
    }
    for (int y = pos.y + 1; y <= 8; y++) {
        Position p{pos.x, y};
        l.append(p);
    }

    return l;
}
