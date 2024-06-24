#include "replay.h"
#include <QDateTime>
#include <QDir>
#include <QRegularExpression>

Replay::Replay(QObject *parent) : QObject(parent) {}

/**
 * 写replay模式，每次board.pieceMoved信号触发后，调用这里的addMovement记录移动
 * @param color
 */
Replay::Replay(QObject *parent, Piece_Color color) : Replay(parent) {
    selfColor = color;
}

/**
 * 读replay模式，调用getMovement获取每步
 * @param filename
 */
Replay::Replay(QObject *parent, QString &filename) : Replay(parent) {
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

void Replay::setSelfName(QString newSelfName) {
    selfName = newSelfName;
}

void Replay::setOppName(QString newOppName) {
    oppName = newOppName;
}

QString makeValidFilename(QString input) {
    // Define a regular expression that matches invalid characters
    static QRegularExpression invalidChars("[\\\\/:\\*\\?\"<>\\|]");

    // Replace invalid characters with an underscore
    QString sanitized = input;
    sanitized.replace(invalidChars, "_");

    // Trim leading and trailing whitespace
    sanitized = sanitized.trimmed();

    // Limit the filename length to 255 characters
    const int maxLength = 255;
    if (sanitized.length() > maxLength) {
        sanitized = sanitized.left(maxLength);
    }

    return sanitized;
}

void Replay::replaySave() {
    QDir dir;
    if (!dir.exists("./replay")) {
        dir.mkdir("./replay");
    }
    // 走两步棋以上的才记录repaly
    if (movementList.length() > 2) {
        QString filename = QString("(%1 vs %2) %3.rep").arg(selfName, oppName, QDateTime::currentDateTime().toString("[yyyy.MM.dd] hh-mm-ss"));
        QFile file("./replay/" + makeValidFilename(filename));
        if (file.open(QFile::WriteOnly)) {
            QTextStream stream(&file);
            stream << QChar(selfColor) << "\n";
            foreach (Movement m, movementList) {
                stream << Movement::toString(m) << "\n";
            }
            file.close();
        }
    }
}

Piece_Color Replay::getSelfColor() {
    return selfColor;
}
