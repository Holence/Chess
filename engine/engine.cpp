#include "engine.h"
#include <QDebug>

Engine::Engine() {
    board = new Cell[64]{nullptr};
    state = GameState::Unfinished;
}

Engine::~Engine() { delete board; }

Piece *Engine::getPiece(Position pos) {}

QList<Position> Engine::getPossibleMove(Position pos) {
    Piece *p = getPiece(pos);
    return p->getPossibleMove(board, pos);
}
