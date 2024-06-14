#include "piece.h"

Piece::Piece(Piece_Type type, Piece_Color color) {
    this->type = type;
    this->color = color;
}

Piece_Color Piece::getColor() { return color; }
