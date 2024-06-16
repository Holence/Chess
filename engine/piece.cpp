#include "piece.h"

Piece::Piece(Piece_Color color, Position pos) {
    this->color = color;
    this->pos = pos;
}

Piece_Color Piece::getColor() { return color; }

Piece_Type Piece::getType() { return type; }

Position Piece::getPos() { return pos; }

void Piece::setType(Piece_Type type) {
    this->type = type;
}

void Piece::setColor(Piece_Color color) {
    this->color = color;
}

void Piece::setPos(Position pos) {
    this->pos = pos;
}

Piece_Color flipPieceColor(Piece_Color color) {
    return color == Piece_Color::White ? Piece_Color::Black : Piece_Color::White;
}
