#ifndef REPLAY_H
#define REPLAY_H

#include "engine/piece.h"
#include <QFile>
#include <QTextStream>

class Replay {
public:
    struct Movement {
        Movement() {}
        Position pos_from, pos_to;
        Piece_Type promteType = Piece_Type::null;
    };
    Replay(QFile::OpenModeFlag mode);
    Replay(Piece_Color color);
    Replay(QString &filename);
    void addMovement(Position pos_from, Position pos_to, Piece_Type promteType);
    void replaySave();

    Piece_Color getSelfColor();
    QList<Movement> getMovementList();

private:
    QFile::OpenModeFlag mode;
    QList<Movement> movementList;
    QMap<char, Piece_Type> typeMap;
    Piece_Color selfColor;
};

#endif // REPLAY_H
