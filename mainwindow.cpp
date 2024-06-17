#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), engine() {
    ui->setupUi(this);

    WhiteIcon[Piece_Type::King] = ":/img/wk.png";
    BlackIcon[Piece_Type::King] = ":/img/bk.png";
    WhiteIcon[Piece_Type::Rook] = ":/img/wr.png";
    BlackIcon[Piece_Type::Rook] = ":/img/br.png";
    WhiteIcon[Piece_Type::Queen] = ":/img/wq.png";
    BlackIcon[Piece_Type::Queen] = ":/img/bq.png";
    WhiteIcon[Piece_Type::Knight] = ":/img/wn.png";
    BlackIcon[Piece_Type::Knight] = ":/img/bn.png";
    WhiteIcon[Piece_Type::Bishop] = ":/img/wb.png";
    BlackIcon[Piece_Type::Bishop] = ":/img/bb.png";
    WhiteIcon[Piece_Type::Pawn] = ":/img/wp.png";
    BlackIcon[Piece_Type::Pawn] = ":/img/bp.png";

    // draw board
    ui->widgetBoard->setFixedSize(QSize(800, 800));
    ui->widgetBoard->setStyleSheet(
        "#widgetBoard {border-image: url(:/img/bg.png) 0 0 0 0 stretch stretch;}"
        "CellButton {background-color: rgba(0,0,0,0);}");
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
            btn->setIconSize(QSize(100, 100));
            btn->setFixedSize(QSize(100, 100));
            connect(btn, &CellButton::leftClicked, this, &MainWindow::cellSelected);
            connect(btn, &CellButton::rightClicked, this, &MainWindow::cellCanceled);
            grid->addWidget(btn, 8 - y, x - 1);
            cellArray[convertPosToIndex(pos)] = btn;
        }
    }

    // Filter
    connect(ui->actionBasicFilter, &QAction::triggered, this, [this] {
        if (selectedCell) {
            QList<Position> l = engine.getBasicFilteredMove(translatePos(selectedCell->getPos()));
            foreach (Position pos, l) {
                CellButton *btn = getCellBtn(pos);
                btn->setStyleSheet("background-color: rgba(255,0,255,63);");
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

    connect(ui->actionSingle_New_Game, &QAction::triggered, this, &MainWindow::SinglePlayerGame);
    connect(ui->actionResign, &QAction::triggered, this, &MainWindow::GameOver);
    connect(this, &MainWindow::gameEnded, this, &MainWindow::GameOver);

    ui->actionSingle_New_Game->setDisabled(false);
    ui->widgetBoard->setDisabled(true);
    ui->actionResign->setDisabled(true);
}

MainWindow::~MainWindow() {
    delete ui;
    delete cellArray;
}

void MainWindow::SinglePlayerGame() {
    // TODO select color box
    selfColor = Piece_Color::White;
    boardFilpped = true;
    engine.newGame();

    drawBoard();

    selectedCell = nullptr;

    // 单人游戏 移动棋子后更换selfColor
    connect(this, &MainWindow::pieceMoved, this, [this] { selfColor = flipPieceColor(selfColor); });

    ui->actionSingle_New_Game->setDisabled(true);
    ui->actionResign->setDisabled(false);
    ui->widgetBoard->setDisabled(false);
}

void MainWindow::GameOver() {
    QString s;

    switch (engine.getGameState()) {
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

    ui->actionSingle_New_Game->setDisabled(false);
    ui->actionResign->setDisabled(true);
    ui->widgetBoard->setDisabled(true);
}

void MainWindow::cellSelected(Position pos) {
    CellButton *current_select_btn = getCellBtn(pos);

    // 之前有选中有效的棋子
    if (selectedCell) {
        // 如果当前点的是可行的走位，则移动棋子
        // TODO Pawn Promotion Menu

        if (movableCellList.contains(current_select_btn)) {
            Position orig_pos = selectedCell->getPos();
            // TODO 改为标准化的字符串接口
            GameState state = engine.nextGameState(translatePos(orig_pos), translatePos(pos));
            updateCellIcon(orig_pos);
            updateCellIcon(pos);

            // 为了那狗屎的EnPassant，我也懒得设计其他接口，也不想每次都对整个棋盘全部刷新，这里额外刷新orig_pos左右两侧的格子🤣
            if (orig_pos.x > 1)
                updateCellIcon(Position{orig_pos.x - 1, orig_pos.y});
            if (orig_pos.x < 8)
                updateCellIcon(Position{orig_pos.x + 1, orig_pos.y});

            emit pieceMoved();
            cellCanceled();
            if (state == GameState::WhiteWin or state == GameState::BlackWin or state == GameState::Draw) {
                emit gameEnded();
            }
        } else {
            cellCanceled();
        }
    }
    //
    else {
        Piece *p = engine.getPiece(translatePos(pos));
        // TODO 存储Piece*，如果是Pawn，落子时就要判断是否要Promote
        // 如果选中棋子 且 选中了己方的棋子
        if (p and p->getColor() == selfColor) {
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
                    btn->setStyleSheet("background-color: rgba(255,255,0,63);");
                    movableCellList.append(btn);
                }
            }
        }
    }
}

void MainWindow::cellCanceled() {
    if (selectedCell) {
        foreach (CellButton *btn, movableCellList) {
            btn->setStyleSheet("background-color: rgba(0,0,0,0);");
        }
        movableCellList.clear();
        foreach (CellButton *btn, filteredCellList) {
            btn->setStyleSheet("background-color: rgba(0,0,0,0);");
        }
        filteredCellList.clear();
        selectedCell = nullptr;
    }
}

void MainWindow::updateCellIcon(Position pos) {
    Piece *p = engine.getPiece(translatePos(pos));
    if (p) {
        if (p->getColor() == Piece_Color::White) {
            getCellBtn(pos)->setIcon(QIcon(WhiteIcon.value(p->getType(), QString())));
        } else {
            getCellBtn(pos)->setIcon(QIcon(BlackIcon.value(p->getType(), QString())));
        }
    } else {
        getCellBtn(pos)->setIcon(QIcon(QString()));
    }
}

CellButton *MainWindow::getCellBtn(Position pos) {
    return cellArray[convertPosToIndex(pos)];
}

Position MainWindow::translatePos(Position pos) {
    if (!boardFilpped) {
        return pos;
    } else {
        return flipSide(pos);
    }
}

QList<Position> MainWindow::translatePosList(QList<Position> posList) {
    QList<Position> l;
    foreach (Position pos, posList) {
        l.append(translatePos(pos));
    }
    return l;
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
