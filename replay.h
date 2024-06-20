#ifndef REPLAY_H
#define REPLAY_H

#include "engine/piece.h"
#include <QFile>
#include <QTextStream>

class Replay {
public:
    Replay();
    Replay(Piece_Color color);
    Replay(QString &filename);
    void addMovement(Movement m);
    void replaySave();

    Piece_Color getSelfColor();
    QList<Movement> getMovementList();

private:
    QFile::OpenModeFlag mode;
    QList<Movement> movementList;
    Piece_Color selfColor;
};

#endif // REPLAY_H
