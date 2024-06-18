#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), engine() {
    ui->setupUi(this);

    WhiteIcon[Piece_Type::king] = ":/img/wk.png";
    BlackIcon[Piece_Type::king] = ":/img/bk.png";
    WhiteIcon[Piece_Type::rook] = ":/img/wr.png";
    BlackIcon[Piece_Type::rook] = ":/img/br.png";
    WhiteIcon[Piece_Type::queen] = ":/img/wq.png";
    BlackIcon[Piece_Type::queen] = ":/img/bq.png";
    WhiteIcon[Piece_Type::knight] = ":/img/wn.png";
    BlackIcon[Piece_Type::knight] = ":/img/bn.png";
    WhiteIcon[Piece_Type::bishop] = ":/img/wb.png";
    BlackIcon[Piece_Type::bishop] = ":/img/bb.png";
    WhiteIcon[Piece_Type::pawn] = ":/img/wp.png";
    BlackIcon[Piece_Type::pawn] = ":/img/bp.png";

    // draw board
    ui->widgetBoard->setFixedSize(QSize(800, 800));
    ui->widgetBoard->setStyleSheet(
        "#widgetBoard {border-image: url(:/img/bg.png) 0 0 0 0 stretch stretch;}");
    QGridLayout *grid = new QGridLayout(ui->widgetBoard);
    grid->setMargin(0);
    grid->setSpacing(0);
    ui->widgetBoard->setLayout(grid);

    // dynamic add CellButton
    cellArray = new CellButton *[64];
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            Position pos{x, y};
            CellButton *btn = new CellButton(pos);
            connect(btn, &CellButton::leftClicked, this, &MainWindow::cellSelected);
            connect(btn, &CellButton::rightClicked, this, &MainWindow::cellCanceled);
            grid->addWidget(btn, 8 - y, x - 1);
            cellArray[pos.toIndex()] = btn;
        }
    }

    // Filter
    connect(ui->actionBasicFilter, &QAction::triggered, this, [this] {
        if (selectedCell) {
            QList<Position> l = translatePosList(engine.getBasicFilteredMove(translatePos(selectedCell->getPos())));
            foreach (Position pos, l) {
                CellButton *btn = getCellBtn(pos);
                btn->paintColor(255, 0, 255, 63);
                filteredCellList.append(btn);
            }
        }
    });

    // Flip Board
    connect(ui->actionFlip_Board, &QAction::triggered, this, [this] {
        boardFilpped = !boardFilpped;
        drawBoard();
        cellCanceled();
    });

    connect(ui->actionSingle_New_Game, &QAction::triggered, this, &MainWindow::startSinglePlayerGame);
    connect(ui->actionResign, &QAction::triggered, this, [this] {
        if (selfColor == Piece_Color::White)
            return GameOver(GameState::BlackWin);
        else
            return GameOver(GameState::WhiteWin);
    });
    connect(this, &MainWindow::gameEnded, this, &MainWindow::GameOver);

    ui->actionSingle_New_Game->setEnabled(true);
    ui->widgetBoard->setDisabled(true);
    ui->actionResign->setDisabled(true);

    // Replay
    replay = nullptr;
    connect(ui->actionLoad_Replay, &QAction::triggered, this, &MainWindow::startReplay);
    connect(ui->actionNext_Move, &QAction::triggered, this, &MainWindow::ReplayNextMove);
    connect(ui->actionPrevious_Move, &QAction::triggered, this, &MainWindow::ReplayPreviousMove);
    ui->actionNext_Move->setDisabled(true);
    ui->actionPrevious_Move->setDisabled(true);

    // TODO Replay
    // TODO Online Match
    // TODO Chat
    // TODO Taunt
    // TODO Sound/BGM
}

MainWindow::~MainWindow() {
    delete ui;
    delete cellArray;
    if (replay) {
        replay->replayEnd();
        delete replay;
    }
}

void MainWindow::startSinglePlayerGame() {
    // TODO select color box
    selfColor = Piece_Color::White;
    if (selfColor == Piece_Color::Black)
        boardFilpped = true;

    engine.newGame();

    drawBoard();
    replay = new Replay(selfColor);

    selectedCell = nullptr;

    // 单人游戏 移动棋子后更换selfColor
    connect(this, &MainWindow::pieceMoved, this, [this] { selfColor = flipPieceColor(selfColor); });

    ui->actionSingle_New_Game->setDisabled(true);
    ui->actionResign->setEnabled(true);
    ui->widgetBoard->setEnabled(true);
}

void MainWindow::startReplay() {
    if (replay != nullptr) {
        replay->replayEnd();
        delete replay;
        replay = nullptr;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Open Replay File", "./replay", "Replay (*.rep)");
    if (!filename.isEmpty()) {
        replay = new Replay(filename);

        selfColor = replay->getSelfColor();
        if (selfColor == Piece_Color::Black)
            boardFilpped = true;
        engine.newGame();
        drawBoard();

        ui->actionNext_Move->setEnabled(true);
        ui->actionPrevious_Move->setEnabled(true);
        ui->widgetBoard->setEnabled(true);
    }
}

// TODO 能不能把Replay、SinglePlay、NetPlay分开开啊？放一个文件里乱死了……
void MainWindow::ReplayNextMove() {
    Replay::Movement m = replay->getMovement(1);
    qDebug() << m.pos_from.toString();
}

void MainWindow::ReplayPreviousMove() {
    Replay::Movement m = replay->getMovement(1);
    qDebug() << m.pos_from.toString();
}

void MainWindow::GameOver(GameState state) {
    replay->replayEnd();
    delete replay;
    replay = nullptr;

    QString s;

    switch (state) {
    case GameState::WhiteWin:
        if (selfColor == Piece_Color::White)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::BlackWin:
        if (selfColor == Piece_Color::Black)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::Draw:
        s = QString("Draw");
        break;
    default:
        // Resign
        s = QString("Resigned");
    }
    engine.endGame();
    cellCanceled();

    QMessageBox msg(this);
    msg.setWindowTitle("Game Over");
    msg.setText(s);
    msg.adjustSize();
    msg.exec();

    // 必须disconnect（每次开始游戏都会重复connect，每多connect一次就会多触发一次slot）
    disconnect(this, &MainWindow::pieceMoved, nullptr, nullptr);

    ui->actionSingle_New_Game->setEnabled(true);
    ui->actionResign->setDisabled(true);
    ui->widgetBoard->setDisabled(true);
}

void MainWindow::cellSelected(Position pos) {
    CellButton *current_select_btn = getCellBtn(pos);

    // 之前有选中有效的棋子
    if (selectedCell) {
        // 如果当前点的是可行的走位，则移动棋子

        if (movableCellList.contains(current_select_btn)) {
            Position orig_pos = selectedCell->getPos();

            Piece_Type promoteType;
            if (selectedPiece->getType() == Piece_Type::pawn and ((Pawn *)selectedPiece)->isReadyToPromote()) {
                // Pawn Promote
                promoteType = getPawnPromotion();
            } else {
                promoteType = Piece_Type::null;
            }
            engine.movePiece(translatePos(orig_pos), translatePos(pos), promoteType);

            // replay记录
            replay->addMovement(orig_pos, pos, promoteType);

            // 刷新棋盘画面
            updateCellIcon(orig_pos);
            updateCellIcon(pos);
            if (selectedPiece->getType() != Piece_Type::king) {
                // 为了那狗屎的EnPassant，我也懒得设计其他接口，也不想每次都对整个棋盘全部刷新，这里额外刷新orig_pos左右两侧的格子🤣
                if (orig_pos.x > 1)
                    updateCellIcon(Position{orig_pos.x - 1, orig_pos.y});
                if (orig_pos.x < 8)
                    updateCellIcon(Position{orig_pos.x + 1, orig_pos.y});
            } else {
                // 还有王车易位，直接刷新一整行算了
                for (int i = 1; i <= 8; i++) {
                    updateCellIcon(Position{i, orig_pos.y});
                }
            }
            cellCanceled();

            // 检查是否game over
            GameState state = engine.checkGameState(selfColor);
            if (state == GameState::WhiteWin or state == GameState::BlackWin or state == GameState::Draw) {
                emit gameEnded(state);
            }

            emit pieceMoved();

        } else {
            cellCanceled();
        }
    }
    //
    else {
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
                    // 标为黄色
                    CellButton *btn = getCellBtn(pos);
                    btn->paintColor(255, 255, 0, 63);
                    movableCellList.append(btn);
                }
            }
        }
    }
}

void MainWindow::cellCanceled() {
    if (selectedCell) {
        foreach (CellButton *btn, movableCellList) {
            btn->clearColor();
        }
        movableCellList.clear();
        foreach (CellButton *btn, filteredCellList) {
            btn->clearColor();
        }
        filteredCellList.clear();
        selectedCell = nullptr;
    }
}

void MainWindow::updateCellIcon(Position pos) {
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

CellButton *MainWindow::getCellBtn(Position pos) {
    return cellArray[pos.toIndex()];
}

/**
 * 因为engine和client间通信的pos统一都是白下黑上的标准
 *
 * 所有涉及给engine发pos、从engine获取pos的操作
 *
 * 以及给对手client发pos、收取对手client的pos
 *
 * 都需要经过这个函数进行转义
 * @param pos
 * @return
 */
Position MainWindow::translatePos(Position pos) {
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
QList<Position> MainWindow::translatePosList(QList<Position> posList) {
    QList<Position> l;
    foreach (Position pos, posList) {
        l.append(translatePos(pos));
    }
    return l;
}

Piece_Type MainWindow::getPawnPromotion() {
    QMap<Piece_Type, QString> *iconMap;
    if (selectedPiece->getColor() == Piece_Color::White)
        iconMap = &WhiteIcon;
    else
        iconMap = &BlackIcon;
    QDialog dlg(this);
    dlg.setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);}");
    QHBoxLayout layout;
    Piece_Type typeArray[] = {Piece_Type::queen, Piece_Type::rook, Piece_Type::knight, Piece_Type::bishop};
    for (int i = 0; i < 4; i++) {
        QPushButton *b = new QPushButton(&dlg);
        connect(b, &QPushButton::clicked, &dlg, [&, i] { dlg.done(i); });
        b->setIconSize(QSize(100, 100));
        b->setFixedSize(QSize(100, 100));
        b->setIcon(QIcon(iconMap->value(typeArray[i])));
        layout.addWidget(b);
    }
    dlg.setLayout(&layout);
    int choose = dlg.exec();
    return typeArray[choose];
}

/**
 * 开始游戏的时候，绘制整个棋盘的棋子
 *
 * 实战中不用这个函数，实战中动态地移动、删除棋子（见cellSelected中调用updateCellIcon()）
 */
void MainWindow::drawBoard() {
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            updateCellIcon(Position{x, y});
        }
    }
}
