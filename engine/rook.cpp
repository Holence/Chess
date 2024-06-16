#include "rook.h"

Rook::Rook(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::Rook;
}

QList<Position> Rook::getBasicMove() {
    QList<Position> l;
    for (int x = pos.x - 1; x >= 1; x--) {
        l.append(Position{x, pos.y});
    }
    for (int x = pos.x + 1; x <= 8; x++) {
        l.append(Position{x, pos.y});
    }
    for (int y = pos.y - 1; y >= 1; y--) {
        l.append(Position{pos.x, y});
    }
    for (int y = pos.y + 1; y <= 8; y++) {
        l.append(Position{pos.x, y});
    }

    return l;
}
