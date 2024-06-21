#include "replay.h"
#include <QDateTime>
#include <QDir>
Replay::Replay() {
}

/**
 * 写replay模式，每次board.pieceMoved信号触发后，调用这里的addMovement记录移动
 * @param color
 */
Replay::Replay(Piece_Color color) : Replay() {
    selfColor = color;
}

/**
 * 读replay模式，调用getMovement获取每步
 * @param filename
 */
Replay::Replay(QString &filename) : Replay() {
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);

        if (stream.readLine() == "w")
            selfColor = Piece_Color::White;
        else
            selfColor = Piece_Color::Black;
        while (!stream.atEnd()) {
            QString s = stream.readLine();
            movementList.append(Movement::fromString(s));
        }
        file.close();
    }
}

void Replay::addMovement(Movement m) {
    movementList.append(m);
}

QList<Movement> Replay::getMovementList() {
    return movementList;
}

void Replay::replaySave() {
    QDir dir;
    if (!dir.exists("./replay")) {
        dir.mkdir("./replay");
    }
    QFile file("./replay/" + QDateTime::currentDateTime().toString("[yyyy.MM.dd] hh-mm-ss") + ".rep");
    if (file.open(QFile::WriteOnly)) {
        QTextStream stream(&file);
        stream << QChar(selfColor) << "\n";
        foreach (Movement m, movementList) {
            stream << Movement::toString(m) << "\n";
        }
        file.close();
    }
}

Piece_Color Replay::getSelfColor() {
    return selfColor;
}
