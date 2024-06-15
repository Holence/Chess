#include "board.h"

#include "king.h"
#include <QDebug>
Board::Board() {
    board = new Cell[64]{nullptr};
    Piece *p;
    p = new King(Piece_Type::King, Piece_Color::White);
    WhitePieces.append(p);
    putPiece(p, Position{1, 3});

    p = new King(Piece_Type::King, Piece_Color::Black);
    BlackPieces.append(p);
    putPiece(p, Position{6, 8});
}

Board::~Board() {
    delete board;
    for (int i = 0; i < WhitePieces.length(); i++) {
        delete WhitePieces.at(i);
    }
    for (int i = 0; i < BlackPieces.length(); i++) {
        delete BlackPieces.at(i);
    }
    for (int i = 0; i < WhiteDeadPieces.length(); i++) {
        delete WhiteDeadPieces.at(i);
    }
    for (int i = 0; i < BlackDeadPieces.length(); i++) {
        delete BlackDeadPieces.at(i);
    }
}

void Board::movePiece(Position from, Position to) {
    // TODO Pawn Promotion?
    Piece *p_from = getPiece(from);
    Piece *p_to = getPiece(to);
    qInfo() << "Check remove?";
    if (p_to != nullptr) {
        if (p_to->getColor() == Piece_Color::White)
            WhiteDeadPieces.append(p_to);
        else
            BlackDeadPieces.append(p_to);
        qInfo() << "Removed";
    }
    putPiece(nullptr, from);
    putPiece(p_from, to);
}

QList<Position> Board::getPossibleMove(Position pos) {
    // TODO filter with board
    Piece *p = getPiece(pos);
    if (p != nullptr)
        return p->getPossibleMove(pos);
    else {
        return QList<Position>();
    }
}

Piece *Board::getPiece(Position pos) { return board[convertPosToIndex(pos)]; }

void Board::putPiece(Piece *p, Position pos) { board[convertPosToIndex(pos)] = p; }
