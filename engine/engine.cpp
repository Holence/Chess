#include "engine.h"
#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "queen.h"
#include "rook.h"

// 白棋大写，黑棋小写，白下黑上
char chessBoardCode[] =
    "rnbqkbnr"
    "pppppppp"
    "........"
    "........"
    "........"
    "........"
    "PPPPPPPP"
    "RNBQKBNR";

Engine::Engine(bool RTS_mode) {
    this->RTS_mode = RTS_mode;

    board = new Cell[64];
    std::fill_n(board, 64, nullptr);

    Piece_Color color;
    QList<Piece *> *PiecesList;
    Piece **king;
    Position pos;
    Piece *p;
    int index = -1;
    // chessBoardCode的字符串顺序不能从头读到尾，得分片读
    // 为了支持自定义开局状态，这里多了这么多判断和赋值，简直就是简直，卑鄙的浪费啊……
    for (int i = 56; i >= 0; i -= 8) {
        for (int j = 0; j < 8; j++) {
            index++;
            char c = chessBoardCode[i + j];
            if (c == '.')
                continue;

            pos = Position::fromIndex(index);
            if (std::isupper(c)) {
                c = std::tolower(c);
                color = Piece_Color::White;
                PiecesList = &WhitePieces;
                king = &WhiteKing;
            } else {
                color = Piece_Color::Black;
                PiecesList = &BlackPieces;
                king = &BlackKing;
            }
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
                p = new Pawn(color, pos);
                break;
            default:
                break;
            }

            (*PiecesList).append(p);
            putPiece(p, pos);
        }
    }
}

Engine::~Engine() {
    delete[] board;
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
}

/**
 * color颜色的一方走一步后，来检查游戏状态
 * @param color
 * @return
 */
GameState Engine::checkGameState(Piece_Color color) {
    Piece_Color oppColor = flipPieceColor(color);
    QList<Piece *> l;
    GameState WinState;
    if (color == Piece_Color::White) {
        l = BlackPieces;
        WinState = GameState::WhiteWin;
    } else {
        l = WhitePieces;
        WinState = GameState::BlackWin;
    }

    if (!RTS_mode) {

        // 移动一步后，自己不可能被将，只需要查看对方的状态
        // 如果对方没有棋能动
        //    如果对方被将，则自己赢了
        //    如果对方没被将，则平局
        // 如果对方有棋能动，如果不是特殊情况的平局，则继续

        beingCheckmated = isBeingCheckmated(oppColor);
        bool hasMoveChance = false;
        foreach (Piece *p, l) {
            if (!getMovablePos(p).isEmpty()) {
                hasMoveChance = true;
                break;
            }
        }
        if (hasMoveChance) {
            if (checkOtherDraw())
                return GameState::Draw;
            else
                return GameState::Unfinished;
        } else {
            if (beingCheckmated) {
                // 对方输了
                return WinState;
            } else {
                return GameState::Draw;
            }
        }
    } else {
        if (l.isEmpty()) {
            return WinState;
        } else {
            return GameState::Unfinished;
        }
    }
}

bool Engine::checkOtherDraw() {
    // Other Draw
    // https://www.chess.com/terms/draw-chess
    // https://www.chess.com/forum/view/endgames/kingbishop-vs-kingknight-not-forced-draw
    // Must be Draw:
    // King vs King, or
    // King vs King & Bishop, or
    // King Vs King & Knight, or
    // King & Bishop vs King & Bishop if and only if both bishops are on the same coloured squares.
    if (WhitePieces.length() <= 2 and BlackPieces.length() <= 2) {
        QList<Piece *> pieceLeft;
        foreach (Piece *p, WhitePieces) {
            if (p->getType() != Piece_Type::king)
                pieceLeft.append(p);
        }
        foreach (Piece *p, BlackPieces) {
            if (p->getType() != Piece_Type::king)
                pieceLeft.append(p);
        }

        // King vs King, thus Draw
        if (pieceLeft.isEmpty()) {
            return true;
        } else {
            foreach (Piece *p, pieceLeft) {
                // has other Piece, thus not Draw
                if (p->getType() != Piece_Type::knight and p->getType() != Piece_Type::bishop)
                    return false;
            }
            // only has Knight or Bishop
            if (pieceLeft.length() == 1) {
                // only one player has Knight or Bishop, thus Draw
                return true;
            } else {
                // both player has Knight or Bishop
                if (pieceLeft.at(0)->getType() == Piece_Type::bishop and pieceLeft.at(1)->getType() == Piece_Type::bishop and (pieceLeft.at(0)->isDarkColor() == pieceLeft.at(1)->isDarkColor())) {
                    // King & Bishop vs King & Bishop if and only if both bishops are on the same coloured squares, thus Draw
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
    // TODO 还有其他更奇葩的Draw
}

/**
 * 对对方压制的地方，是对方的老王不能走进、需要逃离的区域（这也就意味着要包括横竖斜着将军时King的身后位）
 *
 * 是getMovablePos()的父集
 * @param p
 * @return
 */
QList<Position> Engine::getAttackingPos(Piece *p) {
    QList<Position> l = p->getAttackMove();

    // Knight🐎是不可阻挡的！！
    if (p->getType() == Piece_Type::knight) {
        return l;
    }

    Piece_Color pieceColor = p->getColor();
    Position piecePos = p->getPos();

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

            for (int x = piecePos.x + x_offset, y = piecePos.y + y_offset; true; x += x_offset, y += y_offset) {
                Position pos_check{x, y};
                if (!l_copy.contains(pos_check))
                    break;

                Piece *p_check = getPiece(pos_check);
                if (p_check) {
                    // 遇到己方的，加上这个位置，退出这个方向
                    if (p_check->getColor() == pieceColor) {
                        l.append(pos_check);
                        break;
                    } else {
                        // 遇到对方的，加上这个位置
                        l.append(pos_check);
                        if (!RTS_mode and p_check->getType() == Piece_Type::king)
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
    QList<Position> l = getAttackingPos(p);

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
    if (p->getType() == Piece_Type::pawn) {
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
            if (!getPiece(pos)) {
                l.append(pos);
            } else {
                // 如果getAdditionMove返回了两个，且第一个位置有子挡着了，那第二个也到不了的
                break;
            }
        }
    }

    // 如果是King
    if (p->getType() == Piece_Type::king) {
        if (!RTS_mode) {
            // 则不能走向对方的势力范围
            QList<Position> l_copy = l;
            l.clear();
            foreach (Position pos, l_copy) {
                if (!hasPressure(pos, flipPieceColor(pieceColor)))
                    l.append(pos);
            }
        }

        // 王车易位
        // 王没动过 且 没被将军
        if (((King *)p)->getMoved() == false and !beingCheckmated) {
            // 左边的车
            // 左边三格都空
            if (!getPiece(Position{piecePos.x - 1, piecePos.y}) and !getPiece(Position{piecePos.x - 2, piecePos.y}) and !getPiece(Position{piecePos.x - 3, piecePos.y})) {
                Piece *leftPiece = getPiece({piecePos.x - 4, piecePos.y});
                if (leftPiece and leftPiece->getType() == Piece_Type::rook) {
                    // 左边的车没动过
                    if (((Rook *)leftPiece)->getMoved() == false) {
                        // 左边两格都没有pressure
                        if (!hasPressure(Position{piecePos.x - 1, piecePos.y}, flipPieceColor(pieceColor)) and !hasPressure(Position{piecePos.x - 2, piecePos.y}, flipPieceColor(pieceColor))) {
                            l.append(Position{piecePos.x - 2, piecePos.y});
                        }
                    }
                }
            }
            // 右边的车
            // 右边两格都空
            if (!getPiece(Position{piecePos.x + 1, piecePos.y}) and !getPiece(Position{piecePos.x + 2, piecePos.y})) {
                Piece *rightPiece = getPiece({piecePos.x + 3, piecePos.y});
                if (rightPiece and rightPiece->getType() == Piece_Type::rook) {
                    // 右边的车没动过
                    if (((Rook *)rightPiece)->getMoved() == false) {
                        // 左边两格都没有pressure
                        if (!hasPressure(Position{piecePos.x + 1, piecePos.y}, flipPieceColor(pieceColor)) and !hasPressure(Position{piecePos.x + 2, piecePos.y}, flipPieceColor(pieceColor))) {
                            l.append(Position{piecePos.x + 2, piecePos.y});
                        }
                    }
                }
            }
        }

    } else if (!RTS_mode) {
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
        int index = 0;
        foreach (Position pos_to, l_copy) {
            // 模拟走出一步
            Piece *p_to = getPiece(pos_to);

            // ----------------Pawn----------------
            // 模拟En Passant
            if (p_from->getType() == Piece_Type::pawn) {
                // 作为Pawn，还能斜着走到空的位置，说明p_to一定是EnPassantPawn
                if (pos_from.x != pos_to.x and p_to == nullptr) {
                    p_to = EnPassantPawn;
                }
            }
            // Pawn在对方底线其实也就是 向前走挡住横向的将军 和 斜吃，Promotion的选择并不会额外影响老王的安全
            // ------------------------------------

            // 清除起始位置
            clearPos(pos_from);
            // 吃子
            if (p_to != nullptr) {
                clearPos(p_to->getPos());           // 这里要重新获取p_to的pos，因为可能是EnPassantPawn
                index = (*OppPieces).indexOf(p_to); // 用indexOf、insert保证顺序不变
                (*OppPieces).removeAt(index);
            }
            // 移动到终点位置
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

QList<Position> Engine::getVisibleArea(Piece_Color color) {
    QList<Position> res;
    QList<Piece *> *l;
    if (color == Piece_Color::White)
        l = &WhitePieces;
    else
        l = &BlackPieces;
    foreach (Piece *p, *l) {
        res.append(getMovablePos(p));
        res.append(p->getPos());
    }
    return res;
}

/**
 * 移动棋子
 * @param pos_from 移动的起点
 * @param pos_to 移动的终点，如果终点有棋子，那么就是被吃掉的棋子（EnPassant的情况会特殊处理）
 * @param promoteType 如果是Pawn的Promotion，要传入对应的promoteType，否则传入Piece_Type::null
 * @return 被吃掉的棋子的指针
 */
Piece *Engine::movePiece(Movement m) {
    Position pos_from = m.pos_from;
    Position pos_to = m.pos_to;
    Piece_Type promoteType = m.promoteType;

    Piece *p_from = getPiece(pos_from);
    Piece *p_to = getPiece(pos_to);

    // ----------------Pawn----------------
    if (p_from->getType() == Piece_Type::pawn) {
        Pawn *p = (Pawn *)p_from;
        p->setMoved();

        // 作为Pawn，还能斜着走到空的位置，说明p_to一定是EnPassantPawn
        if (pos_from.x != pos_to.x and p_to == nullptr) {
            // 被吃掉的重新绑定为EnPassantPawn
            p_to = EnPassantPawn;
        }

        // 记录前进两格的Pawn
        if (std::abs(pos_from.y - pos_to.y) == 2) {
            EnPassantPawn = p;
        } else {
            EnPassantPawn = nullptr;
        }

        // 如果有promotion
        if (promoteType != Piece_Type::null) {
            Piece_Color color = p_from->getColor();

            QList<Piece *> *l;
            if (color == Piece_Color::White)
                l = &WhitePieces;
            else
                l = &BlackPieces;

            // 删除Pawn
            (*l).removeOne(p_from);
            delete p_from;

            // 变身为新Piece
            switch (promoteType) {
            case Piece_Type::rook:
                p_from = new Rook(color, pos_from);
                break;
            case Piece_Type::knight:
                p_from = new Knight(color, pos_from);
                break;
            case Piece_Type::bishop:
                p_from = new Bishop(color, pos_from);
                break;
            case Piece_Type::queen:
                p_from = new Queen(color, pos_from);
                break;
            default:
                break;
            }
            (*l).append(p_from);
        }
    } else {
        EnPassantPawn = nullptr;
    }
    // ------------------------------------

    // ----------------King----------------
    if (p_from->getType() == Piece_Type::king) {
        King *p = (King *)p_from;
        p->setMoved();
        // 如果横向走两个，说明是王车易位
        if (std::abs(pos_to.x - pos_from.x) == 2) {
            // 这里只用处理车的走位，王的走位让下面默认的逻辑处理
            if (pos_to.x - pos_from.x < 0) {
                // King往左走
                Rook *leftRook = (Rook *)getPiece(Position{pos_from.x - 4, pos_from.y});
                clearPos(leftRook->getPos());
                putPiece(leftRook, Position{pos_from.x - 1, pos_from.y});
                leftRook->setMoved();
            } else {
                // King往右走
                Rook *rightRook = (Rook *)getPiece(Position{pos_from.x + 3, pos_from.y});
                clearPos(rightRook->getPos());
                putPiece(rightRook, Position{pos_from.x + 1, pos_from.y});
                rightRook->setMoved();
            }
        }
    }
    // ------------------------------------

    // 清除起始位置
    clearPos(pos_from);

    // 吃子
    if (p_to != nullptr) {
        clearPos(p_to->getPos()); // 这里要重新获取p_to的pos，因为可能是EnPassantPawn
        if (p_to->getColor() == Piece_Color::White) {
            WhitePieces.removeOne(p_to);
            WhiteDeadPieces.append(p_to);
            if (RTS_mode and p_to->getType() == Piece_Type::king)
                WhiteKing = nullptr;
        } else {
            BlackPieces.removeOne(p_to);
            BlackDeadPieces.append(p_to);
            if (RTS_mode and p_to->getType() == Piece_Type::king)
                BlackKing = nullptr;
        }
    }

    // 移动到终点位置
    putPiece(p_from, pos_to);

    return p_to;
}

Piece *Engine::getPiece(Position pos) {
    return board[pos.toIndex()];
}

bool Engine::isCheckmating(Piece_Color color) {
    if (!RTS_mode)
        return beingCheckmated;
    else {
        Piece_Color oppColor = flipPieceColor(color);
        if (oppColor == Piece_Color::White)
            return WhiteKing != nullptr and isBeingCheckmated(oppColor);
        else
            return BlackKing != nullptr and isBeingCheckmated(oppColor);
    }
}

bool Engine::isKingAlive(Piece_Color color) {
    if (color == Piece_Color::White)
        return WhiteKing != nullptr;
    else
        return BlackKing != nullptr;
}

/**
 * 更改棋子的位置
 * @param p
 * @param pos
 */
void Engine::putPiece(Piece *p, Position pos) {
    board[pos.toIndex()] = p;
    p->setPos(pos);
}

void Engine::clearPos(Position pos) {
    board[pos.toIndex()] = nullptr;
}

/**
 * 计算某个位置被某方棋子施加的“压力”（被保护程度）
 * @param pos
 * @param color 施加压力的某方
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
        if (getAttackingPos(p).contains(pos)) {
            pressure++;
        }
    }

    return pressure;
}

/**
 * 计算某个位置被某方棋子，是否存在施加的“压力”
 *
 * 逻辑和caclPressure一样，只不过可以早退
 * @param pos
 * @param color 施加压力的某方
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
        if (getAttackingPos(p).contains(pos)) {
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

// TODO AI Bot
// 优化的目标是疆域大小？
// 递归正向推演几层后，使疆域最大的走法为最优解
// 如果不clone整个engine，在原engine上进行推演的话，就得写模拟+恢复，很麻烦啊……
// clone的话，每递归深入一层就要clone一份，但内存不是问题
// 递归的可能性空间（时间）才是最大的问题啊……
