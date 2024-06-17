#include "engine.h"
#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "queen.h"
#include "rook.h"
#include <QDebug>

char blackAtBottom[] = "rnbkqbnrpppppppp";
char whiteAtBottom[] = "rnbqkbnrpppppppp";

Engine::Engine() {
}

Engine::~Engine() {
    endGame();
}

void Engine::newGame() {

    board = new Cell[64]{nullptr};

    placePiece(Piece_Color::White, false);
    placePiece(Piece_Color::Black, true);

    state = checkGameState();
}

void Engine::placePiece(Piece_Color color, bool reversed) {

    QList<Piece *> *PiecesList;
    Piece **king;
    char *posCode;
    if (color == Piece_Color::White) {
        PiecesList = &WhitePieces;
        king = &WhiteKing;
        posCode = whiteAtBottom;
    } else {
        PiecesList = &BlackPieces;
        king = &BlackKing;
        posCode = blackAtBottom;
    }

    Position (*posFunc)(int);
    Pawn::Direction direction;
    if (!reversed) {
        posFunc = convertIndexToPos;
        direction = Pawn::Direction::Up;
    } else {
        posFunc = convertIndexToPosFlip;
        direction = Pawn::Direction::Down;
    }

    Position pos;
    Piece *p;
    for (int i = 0; i < 16; i++) {
        char c = posCode[i];
        pos = posFunc(i);
        switch (c) {
        case 'r':
            p = new Rook(color, pos);
            break;
        case 'n':
            p = new Knight(color, pos);
            break;
        case 'b':
            p = new Bishop(color, pos);
            break;
        case 'q':
            p = new Queen(color, pos);
            break;
        case 'k':
            p = new King(color, pos);
            *king = p;
            break;
        case 'p':
            p = new Pawn(color, pos, direction);
            break;
        default:
            break;
        }
        (*PiecesList).append(p);
        putPiece(p, pos);
    }
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
    // 如果没有棋能动
    //    如果被将，则输了
    //    如果没被将，则平局
    // 如果有棋能动，则继续

    GameState checking_state;

    // 检测白色是否被将军
    bool beingCheckmated = isBeingCheckmated(Piece_Color::White);
    bool hasMoveChance = false;
    foreach (Piece *p, WhitePieces) {
        if (!getMovablePos(p).isEmpty()) {
            hasMoveChance = true;
            break;
        }
    }
    if (hasMoveChance) {
        checking_state = GameState::Unfinished;
        // 这里不能return，要继续判断黑色，可能把白色把黑色将死、将平了
    } else {
        if (beingCheckmated) {
            // 输了
            return GameState::BlackWin;
        } else {
            return GameState::Draw;
        }
    }

    // 如果白色没有被将军（如果白色被将军了，那就不用检查黑色了，因为不可能同时被将的）
    if (!beingCheckmated) {
        // 检测黑色是否被将军
        beingCheckmated = isBeingCheckmated(Piece_Color::Black);
        hasMoveChance = false;
        foreach (Piece *p, BlackPieces) {
            if (!getMovablePos(p).isEmpty()) {
                hasMoveChance = true;
                break;
            }
        }
        if (hasMoveChance) {
            checking_state = GameState::Unfinished;
        } else {
            if (beingCheckmated) {
                // 输了
                return GameState::WhiteWin;
            } else {
                return GameState::Draw;
            }
        }
    }

    // TODO Other Draw 只剩马？
    return checking_state;
}

/**
 * 对对方压制的地方，是对方的老王不能走进、需要逃离的区域（这也就意味着要包括横竖斜着将军时King的身后位）
 *
 * 是getMovablePos()的父集
 * @param p
 * @return
 */
QList<Position> Engine::getSuppressingPos(Piece *p) {
    QList<Position> l = p->getAttackMove();
    Piece_Color pieceColor = p->getColor();
    Position pos = p->getPos();

    // Knight🐎是不可阻挡的！！
    if (p->getType() == Piece_Type::Knight) {
        return l;
    }

    // Rook、Bishop、Queen 不能跨越到其他棋子后方 （**对方的王除外，因为将军的时候对方的王必须逃走**）
    // 也包括横竖斜未被遮挡的区域，最后的位置包括对方的子（可以吃的），也包括己方的子（看着的）
    // Pawn的在这里就只有斜吃
    // 下面把横竖斜都过了一边，也就相当于包括了King、Pawn的走法
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

/**
 * 真正可以落点的区域
 * @param p
 * @return
 */
QList<Position> Engine::getMovablePos(Piece *p) {
    QList<Position> l = getSuppressingPos(p);

    Piece_Color pieceColor = p->getColor();
    Position piecePos = p->getPos();

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

    // Pawn
    if (p->getType() == Piece_Type::Pawn) {
        // 如果斜着的地方有对方的子，则可斜吃
        QList<Position> l_copy = l;
        l.clear();
        foreach (Position pos, l_copy) {
            Piece *p = getPiece(pos);
            if (p) {
                if (p->getColor() != pieceColor)
                    l.append(pos);
            }
        }
        // En passant
        if (EnPassantPawn != nullptr) {
            Position EnPassantPos = EnPassantPawn->getPos();
            foreach (Position pos, l_copy) {
                // 斜吃的x与EnPassantPawn一致，且自身的y与EnPassantPawn一致
                if (pos.x == EnPassantPos.x and piecePos.y == EnPassantPos.y)
                    l.append(pos);
            }
        }

        // 到这里才给出向前进的pos，遇到任何子，都不能走上去
        foreach (Position pos, ((Pawn *)p)->getAdditionMove()) {
            Piece *p = getPiece(pos);
            if (!p) {
                l.append(pos);
            }
        }
    }

    // 如果是King，则不能走向对方的势力范围
    if (p->getType() == Piece_Type::King) {
        QList<Position> l_copy = l;
        l.clear();
        foreach (Position pos, l_copy) {
            if (!hasPressure(pos, flipPieceColor(pieceColor)))
                l.append(pos);
        }
    } else {
        // 被将军时，其他兵只能保护老王
        // 如果没被将军，但顶在老王前面，某些移动的方向也会导致老王被将军，这些位置也不可以去
        // 所以不管是否被将军，都要保证：走出一步后，老王是安全的
        QList<Position> l_copy = l;
        l.clear();
        Position pos_from = piecePos;
        Piece *p_from = p;

        // 里面会模拟吃掉对方的棋子(BlackPieces/WhitePieces会被修改并复原)
        QList<Piece *> *OppPieces;
        if (pieceColor == Piece_Color::White) {
            OppPieces = &BlackPieces;
        } else {
            OppPieces = &WhitePieces;
        }
        int index;
        foreach (Position pos_to, l_copy) {
            // 模拟走出一步
            Piece *p_to = getPiece(pos_to);

            if (p_from->getType() == Piece_Type::Pawn) {
                // 作为Pawn，还能斜着走到空的位置，说明p_to一定是EnPassantPawn
                if (pos_from.x != pos_to.x and p_to == nullptr) {
                    p_to = EnPassantPawn;
                }
            }

            clearPos(pos_from);
            // 吃子
            if (p_to != nullptr) {
                clearPos(p_to->getPos());           // 这里要重新获取p_to的pos，因为可能是EnPassantPawn
                index = (*OppPieces).indexOf(p_to); // 用indexOf、insert保证顺序不变
                (*OppPieces).removeAt(index);
            }
            putPiece(p_from, pos_to);

            // 检测老王是否安全
            if (!isBeingCheckmated(pieceColor)) {
                // 让老王安全的位置才可以走
                l.append(pos_to);
            }

            // 恢复原状
            putPiece(p_from, pos_from);
            clearPos(pos_to);
            if (p_to != nullptr) {
                (*OppPieces).insert(index, p_to); // 用indexOf、insert保证顺序不变
                putPiece(p_to, p_to->getPos());
            }
        }
    }

    return l;
}

QList<Position> Engine::getPossibleMove(Position pos) {
    Piece *p = getPiece(pos);
    if (p != nullptr) {
        return getMovablePos(p);
    } else {
        return QList<Position>();
    }
}

QList<Position> Engine::getBasicFilteredMove(Position pos) {
    Piece *p = getPiece(pos);
    if (p != nullptr) {
        return getSuppressingPos(p);
    } else {
        return QList<Position>();
    }
}

GameState Engine::nextGameState(Position from, Position to) {
    movePiece(from, to);
    state = checkGameState();
    return state;
}

void Engine::movePiece(Position pos_from, Position pos_to) {

    Piece *p_from = getPiece(pos_from);
    Piece *p_to = getPiece(pos_to);

    // ----------------Pawn----------------
    if (p_from->getType() == Piece_Type::Pawn) {
        Pawn *p = (Pawn *)p_from;
        p->setMoved();

        // 作为Pawn，还能斜着走到空的位置，说明p_to一定是EnPassantPawn
        if (pos_from.x != pos_to.x and p_to == nullptr) {
            p_to = EnPassantPawn;
        }

        // 记录前进两格的Pawn
        if (std::abs(pos_from.y - pos_to.y) == 2) {
            EnPassantPawn = p;
        } else {
            EnPassantPawn = nullptr;
        }
    } else {
        EnPassantPawn = nullptr;
    }
    // ------------------------------------

    clearPos(pos_from);
    // 吃子
    if (p_to != nullptr) {
        clearPos(p_to->getPos()); // 这里要重新获取p_to的pos，因为可能是EnPassantPawn
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

void Engine::clearPos(Position pos) {
    board[convertPosToIndex(pos)] = nullptr;
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
        if (getSuppressingPos(p).contains(pos)) {
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
        if (getSuppressingPos(p).contains(pos)) {
            flag = true;
            break;
        }
    }

    return flag;
}

/**
 * 某方是否正在被对方checkmate
 *
 * 逻辑和caclPressure一样，只不过可以早退
 * @param pos
 * @param color
 * @return
 */
bool Engine::isBeingCheckmated(Piece_Color color) {
    if (color == Piece_Color::White) {
        return hasPressure(WhiteKing->getPos(), flipPieceColor(color));
    } else {
        return hasPressure(BlackKing->getPos(), flipPieceColor(color));
    }
}
