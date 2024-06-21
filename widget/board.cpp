#include "board.h"
#include <QDialog>
#include <QGridLayout>

const QMap<Piece_Type, QString> Board::WhiteIcon = {
    {Piece_Type::king, ":/img/wk.png"},
    {Piece_Type::rook, ":/img/wr.png"},
    {Piece_Type::queen, ":/img/wq.png"},
    {Piece_Type::knight, ":/img/wn.png"},
    {Piece_Type::bishop, ":/img/wb.png"},
    {Piece_Type::pawn, ":/img/wp.png"},
};

const QMap<Piece_Type, QString> Board::BlackIcon = {
    {Piece_Type::king, ":/img/bk.png"},
    {Piece_Type::rook, ":/img/br.png"},
    {Piece_Type::queen, ":/img/bq.png"},
    {Piece_Type::knight, ":/img/bn.png"},
    {Piece_Type::bishop, ":/img/bb.png"},
    {Piece_Type::pawn, ":/img/bp.png"},
};

Board::Board(QWidget *parent, Piece_Color selfColor, bool isPlayingMode)
    : QWidget(parent), engine() {

    this->selfColor = selfColor;

    // draw board
    setFixedSize(QSize(800, 800));
    setObjectName("Board");
    setStyleSheet("#Board{border-image: url(:/img/bg.png) 0 0 0 0 stretch stretch;}");
    // 自定义Widget的qss还得加上这个才能生效
    setAttribute(Qt::WA_StyledBackground, true);

    QGridLayout *grid = new QGridLayout(this);
    grid->setMargin(0);
    grid->setSpacing(0);
    setLayout(grid);

    // dynamic add CellButton
    cellArray = new CellButton *[64];
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            Position pos{x, y};
            CellButton *btn = new CellButton(pos);
            // 播放replay的话，点击棋盘不用互动
            if (isPlayingMode) {
                connect(btn, &CellButton::leftClicked, this, &Board::cellSelected);
                connect(btn, &CellButton::rightClicked, this, &Board::cellCanceled);
            }
            grid->addWidget(btn, 8 - y, x - 1);
            cellArray[pos.toIndex()] = btn;
        }
    }

    if (selfColor == Piece_Color::Black)
        boardFilpped = true;
    else
        boardFilpped = false;

    mediaPlayer = new QMediaPlayer(this);

    drawBoard();

    playMedia("qrc:/sound/game-start.mp3");
}

Board::~Board() {
    cellCanceled();
    for (int i = 0; i < 64; i++) {
        cellArray[i]->deleteLater();
    }
    delete cellArray;
}

/**
 * 单人模式要轮转控制黑白双方
 */
void Board::flipSelfColor() {
    selfColor = flipPieceColor(selfColor);
}

/**
 * Replay模式、联机模式传入board，让engine移动棋子
 *
 * 传入的pos都是白下黑上的标准pos，这里只需要对board绘制的部分进行pos转义
 * @param pos_from
 * @param pos_to
 * @param promoteType
 */
void Board::movePiece(Movement m) {
    Position pos_from = m.pos_from;
    Position pos_to = m.pos_to;
    Piece *p_move = engine.getPiece(pos_from);
    QString media_path("");

    if (m.promoteType != Piece_Type::null) {
        media_path = "qrc:/sound/promote.mp3";
    }

    // 标准通信
    Piece *p_eaten = engine.movePiece(m);
    if (p_eaten) {
        media_path = "qrc:/sound/capture.mp3";
        emit pieceEaten(p_eaten);
    }

    // board需要转义过的
    pos_from = translatePos(pos_from);
    pos_to = translatePos(pos_to);

    // 清空上次的痕迹
    foreach (CellButton *btn, traceCellList) {
        btn->clearColor();
    }
    traceCellList.clear();
    // 记录走位留下的痕迹，第一个是from，第二个是to
    traceCellList.append(getCellBtn(pos_from));
    traceCellList.append(getCellBtn(pos_to));

    // 刷新棋盘画面
    updateCellIcon(pos_from);
    updateCellIcon(pos_to);
    if (p_move->getType() != Piece_Type::king) {
        // 为了那狗屎的EnPassant，我也懒得设计其他接口，也不想每次都对整个棋盘全部刷新，这里额外刷新pos_from左右两侧的格子🤣
        if (pos_from.x > 1)
            updateCellIcon(Position{pos_from.x - 1, pos_from.y});
        if (pos_from.x < 8)
            updateCellIcon(Position{pos_from.x + 1, pos_from.y});
    } else {
        // 还有王车易位，直接刷新一整行算了
        for (int i = 1; i <= 8; i++) {
            updateCellIcon(Position{i, pos_from.y});
        }
        // 王车易位
        if (std::abs(pos_to.x - pos_from.x) == 2) {
            media_path = "qrc:/sound/castle.mp3";
        }
    }

    // 检查是否game over
    GameState state = engine.checkGameState(p_move->getColor());
    if (engine.getBeingCheckmated()) {
        media_path = "qrc:/sound/move-check.mp3";
    }

    if (media_path.isEmpty()) {
        media_path = "qrc:/sound/move-opponent.mp3";
    }

    playMedia(media_path);
    if (state == GameState::WhiteWin or state == GameState::BlackWin or state == GameState::Draw) {
        emit gameEnded(state);
    }

    cellCanceled();
}

void Board::cellSelected(Position pos) {
    CellButton *current_select_btn = getCellBtn(pos);
    QString media_path("");

    // 之前有选中有效的棋子 且 当前点的是可行的走位，则移动棋子
    if (selectedCell and movableCellList.contains(current_select_btn)) {
        Position pos_from = selectedCell->getPos();
        Position pos_to = pos;

        Piece_Type promoteType;
        if (selectedPiece->getType() == Piece_Type::pawn and ((Pawn *)selectedPiece)->isReadyToPromote()) {
            // Pawn Promote
            promoteType = getPawnPromotion();
            media_path = "qrc:/sound/promote.mp3";
        } else {
            promoteType = Piece_Type::null;
        }

        Movement m{translatePos(pos_from), translatePos(pos_to), promoteType};
        Piece *p_eaten = engine.movePiece(m);
        if (p_eaten) {
            media_path = "qrc:/sound/capture.mp3";
            emit pieceEaten(p_eaten);
        }

        // 清空上次的痕迹
        foreach (CellButton *btn, traceCellList) {
            btn->clearColor();
        }
        traceCellList.clear();
        // 记录走位留下的痕迹，第一个是from，第二个是to
        traceCellList.append(getCellBtn(pos_from));
        traceCellList.append(current_select_btn);

        // 刷新棋盘画面
        updateCellIcon(pos_from);
        updateCellIcon(pos_to);
        if (selectedPiece->getType() != Piece_Type::king) {
            // 为了那狗屎的EnPassant，我也懒得设计其他接口，也不想每次都对整个棋盘全部刷新，这里额外刷新pos_from左右两侧的格子🤣
            if (pos_from.x > 1)
                updateCellIcon(Position{pos_from.x - 1, pos_from.y});
            if (pos_from.x < 8)
                updateCellIcon(Position{pos_from.x + 1, pos_from.y});
        } else {
            // 还有王车易位，直接刷新一整行算了
            for (int i = 1; i <= 8; i++) {
                updateCellIcon(Position{i, pos_from.y});
            }
            // 王车易位
            if (std::abs(pos_to.x - pos_from.x) == 2) {
                media_path = "qrc:/sound/castle.mp3";
            }
        }

        // 检查是否game over
        GameState state = engine.checkGameState(selectedPiece->getColor());
        if (engine.getBeingCheckmated()) {
            media_path = "qrc:/sound/move-check.mp3";
        }

        if (media_path.isEmpty()) {
            media_path = "qrc:/sound/move-self.mp3";
        }
        playMedia(media_path);

        emit pieceMoved(m);
        if (state == GameState::WhiteWin or state == GameState::BlackWin or state == GameState::Draw) {
            emit gameEnded(state);
        }
        cellCanceled();
    }
    // 否则尝试展示可行的走位
    else {
        cellCanceled();
        selectedPiece = engine.getPiece(translatePos(pos));
        // 如果选中棋子 且 选中了己方的棋子
        if (selectedPiece and selectedPiece->getColor() == selfColor) {
            // 获取可行的走位
            QList<Position> l = translatePosList(engine.getPossibleMove(translatePos(pos)));
            if (l.isEmpty()) {
                // 选的棋子没有可行的走位
                selectedCell = nullptr;
            } else {
                selectedCell = current_select_btn;
                foreach (Position pos, l) {
                    // 标为绿色
                    CellButton *btn = getCellBtn(pos);
                    btn->paintColor(0, 255, 0, 63);
                    movableCellList.append(btn);
                }
            }
        }
    }
}

Piece_Type Board::getPawnPromotion() {
    QMap<Piece_Type, QString> iconMap;
    if (selectedPiece->getColor() == Piece_Color::White)
        iconMap = WhiteIcon;
    else
        iconMap = BlackIcon;

    QDialog *dlg = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    dlg->setWindowTitle("Pawn Promotion");
    dlg->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);}");
    QHBoxLayout layout;
    Piece_Type typeArray[] = {Piece_Type::queen, Piece_Type::rook, Piece_Type::knight, Piece_Type::bishop};
    for (int i = 0; i < 4; i++) {
        QPushButton *b = new QPushButton(dlg);
        connect(b, &QPushButton::clicked, dlg, [dlg, i] { dlg->done(i); });
        b->setIconSize(QSize(100, 100));
        b->setFixedSize(QSize(100, 100));
        b->setIcon(QIcon(iconMap.value(typeArray[i])));
        layout.addWidget(b);
    }
    dlg->setLayout(&layout);
    int choose = dlg->exec();
    return typeArray[choose];
}

void Board::cellCanceled() {
    if (selectedCell) {
        selectedCell = nullptr;

        foreach (CellButton *btn, movableCellList) {
            btn->clearColor();
        }
        movableCellList.clear();
    }
    int a = 63;
    foreach (CellButton *btn, traceCellList) {
        // 标为黄色
        btn->paintColor(255, 255, 0, a);
        a += 64;
    }
}

void Board::updateCellIcon(Position pos) {
    Piece *p = engine.getPiece(translatePos(pos));
    if (p) {
        if (p->getColor() == Piece_Color::White) {
            getCellBtn(pos)->setIcon(QIcon(WhiteIcon.value(p->getType())));
        } else {
            getCellBtn(pos)->setIcon(QIcon(BlackIcon.value(p->getType())));
        }
    } else {
        getCellBtn(pos)->setIcon(QIcon(QString()));
    }
}

CellButton *Board::getCellBtn(Position pos) {
    return cellArray[pos.toIndex()];
}

/**
 * 开始游戏的时候，绘制整个棋盘的棋子
 *
 * 实战中不用这个函数，实战中动态地移动、删除棋子（见cellSelected中调用updateCellIcon()）
 */
void Board::drawBoard() {
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            updateCellIcon(Position{x, y});
        }
    }
}

void Board::flipBoard() {
    boardFilpped = !boardFilpped;
    drawBoard();

    // 翻转后指向的btn也要翻转
    if (!traceCellList.isEmpty()) {
        QList<CellButton *> traceCellList_copy = traceCellList;
        traceCellList.clear();
        foreach (CellButton *btn, traceCellList_copy) {
            btn->clearColor();
            traceCellList.append(getCellBtn(btn->getPos().flipSide()));
        }
    }

    cellCanceled();
}

/**
 * 因为engine和client间通信的、replay保存的pos统一都是白下黑上的标准
 *
 * 只要是 board的pos 与 engine/client/replay 之间的通信 （从Board::pieceMoved()信号传出去、从Board::movePiece()传进来的），都需要经过这个函数进行转义
 *
 * 但engine/client/replay之间是不用转义的（client那边的board会在Board::movePiece()负责转义）
 * @param pos
 * @return
 */
Position Board::translatePos(Position pos) {
    if (!boardFilpped) {
        return pos;
    } else {
        return pos.flipSide();
    }
}

/**
 * 同translatePos
 * @param posList
 * @return
 */
QList<Position> Board::translatePosList(QList<Position> posList) {
    QList<Position> l;
    foreach (Position pos, posList) {
        l.append(translatePos(pos));
    }
    return l;
}

void Board::playMedia(QString path) {
    mediaPlayer->setMedia(QUrl(path));
    mediaPlayer->play();
}
