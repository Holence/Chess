#include "engine.h"
#include "king.h"
#include "queen.h"
#include "rook.h"
#include <QDebug>

char ImWhite[] = "r  qk  r                r  qk  r";
char ImBlack[] = "r  kq  r                r  kq  r";

// char ImWhite[] = "rnbqkbnrpppppppppppppppprnbqkbnr";
// char ImBlack[] = "rnbkqbnrpppppppppppppppprnbkqbnr";

Engine::Engine() {}

Engine::~Engine() {
    endGame();
}

void Engine::newGame(Piece_Color selfColor) {
    this->selfColor = selfColor;

    board = new Cell[64]{nullptr};

    char *posCode;
    if (selfColor == Piece_Color::White)
        posCode = ImWhite;
    else
        posCode = ImBlack;

    Position pos;
    Piece *p;

    for (int i = 0; i < 16; i++) {
        char c = posCode[i];
        pos = convertIndexToPos(i);
        switch (c) {
        case 'r':
            p = new Rook(selfColor, pos);
            break;
        // case 'n':
        //     p = new Knight(selfColor, pos);
        //     break;
        // case 'b':
        // p = new Bishop(selfColor, pos);
        // break;
        case 'q':
            p = new Queen(selfColor, pos);
            break;
        case 'k':
            p = new King(selfColor, pos);
            SelfKing = p;
            break;
        // case 'p':
        // p = new Pawn(selfColor, pos);
        // break;
        default:
            break;
        }
        if (c != ' ') {
            SelfPieces.append(p);
            putPiece(p, pos);
        }
    }

    Piece_Color oppColor = flipPieceColor(selfColor);
    for (int i = 16; i < 32; i++) {
        char c = posCode[i];
        pos = convertIndexToPos(i + 32);
        switch (c) {
        case 'r':
            p = new Rook(oppColor, pos);
            break;
        // case 'n':
        //     p = new Knight(oppColor, pos);
        //     break;
        // case 'b':
        // p = new Bishop(oppColor, pos);
        // break;
        case 'q':
            p = new Queen(oppColor, pos);
            break;
        case 'k':
            p = new King(oppColor, pos);
            OppKing = p;
            break;
        // case 'p':
        // p = new Pawn(oppColor, pos);
        // break;
        default:
            break;
        }
        if (c != ' ') {
            OppPieces.append(p);
            putPiece(p, pos);
        }
    }

    state = checkGameState();
}

void Engine::endGame() {
    std::fill_n(board, 64, nullptr);
    for (int i = 0; i < SelfPieces.length(); i++) {
        delete SelfPieces.at(i);
    }
    SelfPieces.clear();
    for (int i = 0; i < OppPieces.length(); i++) {
        delete OppPieces.at(i);
    }
    OppPieces.clear();
    for (int i = 0; i < SelfDeadPieces.length(); i++) {
        delete SelfDeadPieces.at(i);
    }
    SelfDeadPieces.clear();
    for (int i = 0; i < OppDeadPieces.length(); i++) {
        delete OppDeadPieces.at(i);
    }
    OppDeadPieces.clear();
    SelfKing = nullptr;
    OppKing = nullptr;
}

GameState Engine::getGameState() {
    return state;
}

GameState Engine::checkGameState() {
    // 检测己方
    // King没有逃路且只剩King了
    if (getPossibleMove(SelfKing->getPos()).isEmpty() and SelfPieces.length() == 1) {
        // King被将，Lose
        if (hasPressure(SelfKing->getPos(), flipPieceColor(selfColor))) // 其实不可能大于1
            state = GameState::Lose;

        // King没被将，Draw
        else
            state = GameState::Draw;
    }

    // 检测对方
    // King没有逃路且只剩King了
    else if (getPossibleMove(OppKing->getPos()).isEmpty() and OppPieces.length() == 1) {
        // King被将，Lose
        if (hasPressure(OppKing->getPos(), selfColor)) // 其实不可能大于1
            state = GameState::Win;

        // King没被将，Draw
        else
            state = GameState::Draw;
    }

    // TODO CheckMate state 将会导致对方的getStrictFilteredPos有所受限
    // TODO Other Draw 只剩马？
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
        if (p_to->getColor() == selfColor) {
            SelfPieces.removeOne(p_to);
            SelfDeadPieces.append(p_to);
        } else {
            OppPieces.removeOne(p_to);
            OppDeadPieces.append(p_to);
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

/**
 * 更广泛的走位范围，用于计算Pressure
 *
 * 是getStrictFilteredPos的父集
 * @param p
 * @return
 */
QList<Position> Engine::getBasicFilteredPos(Piece *p) {
    // Rook、Bishop、Queen 不能跨越到其他棋子后方 （**对方的王除外，因为将军的时候对方的王必须逃走**）
    // 横竖斜未被遮挡的区域，最后的位置包括对方的（可以吃的），也包括己方的（看着的）
    QList<Position> l = p->getPossibleMove();
    Piece_Color pieceColor = p->getColor();
    Position pos = p->getPos();

    QList<Position> l_copy = l;
    l.clear();

    int offset[3] = {-1, 0, 1};

    for (int i = 0; i < 3; i++) {
        int x_offset = offset[i];

        for (int j = 0; j < 3; j++) {
            int y_offset = offset[j];

            if (x_offset == 0 and y_offset == 0)
                continue;

            for (int x = pos.x + x_offset, y = pos.y + y_offset; true; x += x_offset, y += y_offset) {
                Position pos_check{x, y};
                if (!l_copy.contains(pos_check))
                    break;

                Piece *p = getPiece(pos_check);
                if (p) {
                    // 遇到己方的，加上这个位置，退出这个方向
                    if (p->getColor() == pieceColor) {
                        l.append(pos_check);
                        break;
                    } else {
                        // 遇到对方的，加上这个位置
                        l.append(pos_check);
                        if (p->getType() == Piece_Type::King)
                            // 如果是对方的King，则后方也是势力范围
                            continue;
                        else
                            // 否则，退出这个方向
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
    Piece_Color pieceColor = p->getColor();

    // 不能走到己方的棋子上
    QList<Position> l_copy = l;
    l.clear();
    foreach (Position pos, l_copy) {
        Piece *p = getPiece(pos);
        if (p) {
            if (p->getColor() != pieceColor)
                l.append(pos);
        } else {
            l.append(pos);
        }
    }

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
    if (color == selfColor)
        l = SelfPieces;
    else
        l = OppPieces;

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
    if (color == selfColor)
        l = SelfPieces;
    else
        l = OppPieces;

    bool flag = false;
    foreach (Piece *p, l) {
        if (getBasicFilteredPos(p).contains(pos)) {
            flag = true;
            break;
        }
    }

    return flag;
}
