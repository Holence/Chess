#ifndef ENGINE_H
#define ENGINE_H

#include "piece.h"

// 每个Cell要么是指向Piece的指针，要么是nullptr
#define Cell Piece *

enum class GameState {
    WhiteWin,
    BlackWin,
    Draw,
    Unfinished
};

class Engine {
public:
    Engine();
    ~Engine();

    GameState nextGameState(Position from, Position to);

    QList<Position> getPossibleMove(Position pos);
    QList<Position> getBasicFilteredMove(Position pos);
    Piece *getPiece(Position pos);

    void newGame(Piece_Color colorAtBottom);
    void endGame();

    GameState getGameState();

private:
    GameState state;
    GameState checkGameState();

    Cell *board; // board是指向Cell array的指针
    QList<Piece *> WhitePieces;
    QList<Piece *> WhiteDeadPieces;
    QList<Piece *> BlackPieces;
    QList<Piece *> BlackDeadPieces;

    Piece *WhiteKing;
    Piece *BlackKing;

    void placePiece(Piece_Color color, char *posCode, QList<Piece *> &PiecesList, Piece **king, Position posFunc(int));
    void putPiece(Piece *p, Position pos);
    void movePiece(Position from, Position to);

    int caclPressure(Position pos, Piece_Color color);
    bool hasPressure(Position pos, Piece_Color color);
    bool isBeingCheckmated(Piece_Color color);

    QList<Position> getSuppressingPos(Piece *p);
    QList<Position> getMovablePos(Piece *p);
};

#endif // ENGINE_H
