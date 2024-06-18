#include "replay.h"
#include <QDebug>
#include <QDir>

Replay::Replay(QFile::OpenModeFlag mode) {
    typeMap['p'] = Piece_Type::pawn;
    typeMap['k'] = Piece_Type::king;
    typeMap['q'] = Piece_Type::queen;
    typeMap['r'] = Piece_Type::rook;
    typeMap['b'] = Piece_Type::bishop;
    typeMap['n'] = Piece_Type::knight;
    typeMap['0'] = Piece_Type::null;
    this->mode = mode;
}

Replay::Replay(Piece_Color color) : Replay(QFile::WriteOnly) {
    selfColor = color;
}

Replay::Replay(QString &filename) : Replay(QFile::ReadOnly) {
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

            qDebug() << sl.at(0) << sl.at(1);
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
    m.pos_from = pos_to;
    m.promteType = promteType;
    movementList.append(m);
}

Replay::Movement Replay::getMovement(int index) {
    return movementList.at(index);
}

void Replay::replayEnd() {
    if (mode == QFile::WriteOnly) {
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
    movementList.clear();
}

Piece_Color Replay::getSelfColor() {
    return selfColor;
}
