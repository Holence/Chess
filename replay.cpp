#include "replay.h"
#include <QDir>

Replay::Replay() {
    typeMap['p'] = Piece_Type::pawn;
    typeMap['k'] = Piece_Type::king;
    typeMap['q'] = Piece_Type::queen;
    typeMap['r'] = Piece_Type::rook;
    typeMap['b'] = Piece_Type::bishop;
    typeMap['n'] = Piece_Type::knight;
    typeMap['0'] = Piece_Type::null;
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
            QStringList sl = s.split(" ");
            Movement m;
            m.pos_from = Position::fromString(sl.at(0));
            m.pos_to = Position::fromString(sl.at(1));

            if (sl.length() == 3) {
                m.promteType = typeMap.value(sl.at(2).at(0).unicode());
            }
            movementList.append(m);
        }
        file.close();
    }
}

void Replay::addMovement(Position pos_from, Position pos_to, Piece_Type promteType) {
    Movement m;
    m.pos_from = pos_from;
    m.pos_to = pos_to;
    m.promteType = promteType;
    movementList.append(m);
}

QList<Replay::Movement> Replay::getMovementList() {
    return movementList;
}

void Replay::replaySave() {
    QDir dir;
    if (!dir.exists("./replay")) {
        dir.mkdir("./replay");
    }
    QFile file("./replay/aaa.rep");
    if (file.open(QFile::WriteOnly)) {
        QTextStream stream(&file);
        stream << QChar(selfColor) << "\n";
        foreach (Movement m, movementList) {
            stream << m.pos_from.toString() << " ";
            stream << m.pos_to.toString();
            if (m.promteType != Piece_Type::null) {
                stream << " " << QChar(m.promteType);
            }
            stream << "\n";
        }
        file.close();
    }
}

Piece_Color Replay::getSelfColor() {
    return selfColor;
}
