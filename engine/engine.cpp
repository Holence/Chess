#include "engine.h"

Engine::Engine() : board() { state = GameState::Unfinished; }

Engine::~Engine() {}

GameState Engine::checkGameState() {
    // King
    return GameState::Unfinished;
}

GameState Engine::nextGameState(Position from, Position to) {
    board.movePiece(from, to);
    state = checkGameState();
    return state;
}

QList<Position> Engine::getPossibleMove(Position pos) {
    return board.getPossibleMove(pos);
}
