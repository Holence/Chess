#include "engine.h"
#include "king.h"
#include <QDebug>
Engine::Engine() {}

Engine::~Engine() {
    endGame();
}

void Engine::newGame() {
    board = new Cell[64]{nullptr};

    Piece *p;
    Position pos;

    pos = Position{6, 5};
    p = new King(Piece_Type::King, Piece_Color::White, pos);
    WhitePieces.append(p);
    putPiece(p, pos);
    WhiteKing = p;

    pos = Position{8, 6};
    p = new King(Piece_Type::King, Piece_Color::White, pos);
    WhitePieces.append(p);
    putPiece(p, pos);
    WhiteKing = p;

    pos = Position{8, 8};
    p = new King(Piece_Type::King, Piece_Color::Black, pos);
    BlackPieces.append(p);
    putPiece(p, pos);
    BlackKing = p;

    state = checkGameState();
}

void Engine::endGame() {
    std::fill_n(board, 64, nullptr);
    for (int i = 0; i < WhitePieces.length(); i++) {
        delete WhitePieces.at(i);
    }
    WhitePieces.clear();
    for (int i = 0; i < BlackPieces.length(); i++) {
        delete BlackPieces.at(i);
    }
    BlackPieces.clear();
    for (int i = 0; i < WhiteDeadPieces.length(); i++) {
        delete WhiteDeadPieces.at(i);
    }
    WhiteDeadPieces.clear();
    for (int i = 0; i < BlackDeadPieces.length(); i++) {
        delete BlackDeadPieces.at(i);
    }
    BlackDeadPieces.clear();
    WhiteKing = nullptr;
    BlackKing = nullptr;
}

GameState Engine::getGameState() {
    return state;
}

GameState Engine::checkGameState() {
    // 检测白棋
    // King没有逃路且只剩King了
    if (getPossibleMove(WhiteKing->getPos()).isEmpty() and WhitePieces.length() == 1) {
        // King被将，Lose
        if (hasPressure(WhiteKing->getPos(), Piece_Color::Black)) // 其实不可能大于1
            state = GameState::BlackWin;

        // King没被将，Draw
        else
            state = GameState::Draw;
    }

    // 检测黑棋
    // King没有逃路且只剩King了
    else if (getPossibleMove(BlackKing->getPos()).isEmpty() and BlackPieces.length() == 1) {
        // King被将，Lose
        if (hasPressure(BlackKing->getPos(), Piece_Color::White)) // 其实不可能大于1
            state = GameState::WhiteWin;

        // King没被将，Draw
        else
            state = GameState::Draw;
    }

    // Other Draw 只剩马？
    else {
        state = GameState::Unfinished;
    }
    return state;
}

GameState Engine::nextGameState(Position from, Position to) {
    movePiece(from, to);
    state = checkGameState();
    return state;
}

void Engine::movePiece(Position pos_from, Position pos_to) {
    // TODO Pawn Promotion Menu?
    Piece *p_from = getPiece(pos_from);
    Piece *p_to = getPiece(pos_to);

    board[convertPosToIndex(pos_from)] = nullptr;
    // 吃子
    if (p_to != nullptr) {
        if (p_to->getColor() == Piece_Color::White) {
            WhitePieces.removeOne(p_to);
            WhiteDeadPieces.append(p_to);
        } else {
            BlackPieces.removeOne(p_to);
            BlackDeadPieces.append(p_to);
        }
    }
    putPiece(p_from, pos_to);
}

/**
 *
 * @param pos
 * @return
 */
QList<Position> Engine::getPossibleMove(Position pos) {
    Piece *p = getPiece(pos);
    if (p != nullptr) {
        return getStrictFilteredPos(p);
    } else {
        return QList<Position>();
    }
}

QList<Position> Engine::getBasicFilteredPos(Piece *p) {
    // Rook、Bishop、Queen 不能跨越到其他棋子后方
    // 可以走到对方棋子上
    // 不能走到自己棋子上
    // 横竖斜被遮挡，最后的位置包括对方的、不包括自己的
    QList<Position> l = p->getPossibleMove();
    Piece_Color selfColor = p->getColor();
    Position pos = p->getPos();

    QList<Position> l_copy = l;
    l.clear();

    int offset[3] = {-1, 0, 1};

    for (int i = 0; i < 3; i++) {
        int x_offset = offset[i];

        for (int j = 0; j < 3; j++) {
            int y_offset = offset[j];

            for (int x = pos.x + x_offset, y = pos.y + y_offset; true; x += x_offset, y += y_offset) {
                Position pos_check{x, y};
                if (!l_copy.contains(pos_check))
                    break;

                Piece *p = getPiece(pos_check);
                if (p) {
                    // 遇到自己的，退出这个方向
                    if (p->getColor() == selfColor) {
                        break;
                    } else {
                        // 遇到对方的，加上这个位置，退出这个方向
                        l.append(pos_check);
                        break;
                    }
                } else {
                    // 遇到空位，加上这个位置，继续这个方向
                    l.append(pos_check);
                }
            }
        }
    }

    return l;
}

QList<Position> Engine::getStrictFilteredPos(Piece *p) {
    QList<Position> l = getBasicFilteredPos(p);

    // TODO filter with board
    // - 被将军时其他兵唯一的自保行为
    // - Pawn是否可斜吃
    // - Pawn En passant

    // 如果是King，则不能走向对方的势力范围
    if (p->getType() == Piece_Type::King) {
        QList<Position> l_copy = l;
        l.clear();
        foreach (Position check_pos, l_copy) {
            if (!hasPressure(check_pos, flipPieceColor(p->getColor())))
                l.append(check_pos);
        }
    }

    return l;
}

Piece *Engine::getPiece(Position pos) {
    return board[convertPosToIndex(pos)];
}

/**
 * 更改棋子的位置
 * @param p
 * @param pos
 */
void Engine::putPiece(Piece *p, Position pos) {
    board[convertPosToIndex(pos)] = p;
    p->setPos(pos);
}

/**
 * 计算某个位置被某方棋子施加的“压力”（被保护程度）
 * @param pos
 * @param color
 * @return
 */
int Engine::caclPressure(Position pos, Piece_Color color) {
    QList<Piece *> l;
    if (color == Piece_Color::White)
        l = WhitePieces;
    else
        l = BlackPieces;

    int pressure = 0;
    foreach (Piece *p, l) {
        if (getBasicFilteredPos(p).contains(pos)) {
            pressure++;
        }
    }

    return pressure;
}

/**
 * 计算某个位置被某方棋子，是否存在施加的“压力”（被保护程度）
 *
 * 逻辑和caclPressure一样，只不过可以早退
 * @param pos
 * @param color
 * @return
 */
bool Engine::hasPressure(Position pos, Piece_Color color) {
    QList<Piece *> l;
    if (color == Piece_Color::White)
        l = WhitePieces;
    else
        l = BlackPieces;

    bool flag = false;
    foreach (Piece *p, l) {
        if (getBasicFilteredPos(p).contains(pos)) {
            flag = true;
            break;
        }
    }

    return flag;
}
