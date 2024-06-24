#ifndef REPLAY_H
#define REPLAY_H

#include "engine/piece.h"
#include <QFile>
#include <QObject>
#include <QTextStream>

class Replay : public QObject {
    Q_OBJECT
public:
    Replay(QObject *parent);
    Replay(QObject *parent, Piece_Color color);
    Replay(QObject *parent, QString &filename);
    void addMovement(Movement m);
    void replaySave();

    Piece_Color getSelfColor();
    QList<Movement> getMovementList();

    void setSelfName(QString newSelfName);
    void setOppName(QString newOppName);

private:
    QFile::OpenModeFlag mode;
    QList<Movement> movementList;
    Piece_Color selfColor;
    QString selfName, oppName;
};

#endif // REPLAY_H
