#ifndef PIECE_H
#define PIECE_H

#include "position.h"
#include <QList>
#include <QMap>

enum Piece_Type : char { pawn = 'p',
                         king = 'k',
                         queen = 'q',
                         rook = 'r',
                         bishop = 'b',
                         knight = 'n',
                         null = '0' };

enum Piece_Color : char { White = 'w',
                          Black = 'b' };

const QMap<char, Piece_Type> PieceTypeMap{
    {'p', Piece_Type::pawn},
    {'k', Piece_Type::king},
    {'q', Piece_Type::queen},
    {'r', Piece_Type::rook},
    {'b', Piece_Type::bishop},
    {'n', Piece_Type::knight},
    {'0', Piece_Type::null},
};

struct Movement {
public:
    Position pos_from, pos_to;
    Piece_Type promoteType = Piece_Type::null;

    static Movement fromString(QString s) {
        QStringList sl = s.split(" ");
        Movement m;
        m.pos_from = Position::fromString(sl.at(0));
        m.pos_to = Position::fromString(sl.at(1));

        if (sl.length() == 3) {
            m.promoteType = PieceTypeMap.value(sl.at(2).at(0).unicode());
        }
        return m;
    }

    static QString toString(Movement m) {
        QString s;
        s += m.pos_from.toString() + " " + m.pos_to.toString();
        if (m.promoteType != Piece_Type::null) {
            s += " " + QString(m.promoteType);
        }
        return s;
    }
};

class Piece {
public:
    Piece(Piece_Color color, Position pos);
    virtual QList<Position> getAttackMove() = 0;
    virtual ~Piece() = default;
    Piece_Color getColor();
    Piece_Type getType();
    Position getPos();

    void setType(Piece_Type type);
    void setColor(Piece_Color newColor);
    void setPos(Position pos);
    bool isDarkColor();

protected:
    Piece_Type type;
    Piece_Color color;
    Position pos;
};

Piece_Color flipPieceColor(Piece_Color color);

#endif // PIECE_H
