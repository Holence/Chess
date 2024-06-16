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
    board = new CellButton *[64];
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            Position pos{x, y};
            CellButton *btn = new CellButton(pos);
            btn->setIconSize(QSize(100, 100));
            btn->setFixedSize(QSize(100, 100));
            connect(btn, &CellButton::leftClicked, this, &MainWindow::cellSelected);
            connect(btn, &CellButton::rightClicked, this, &MainWindow::cellCanceled);
            grid->addWidget(btn, 8 - y, x - 1);
            board[convertPosToIndex(pos)] = btn;
        }
    }

    connect(ui->actionSingle_New_Game, &QAction::triggered, this, &MainWindow::SinglePlayerGame);
    connect(ui->actionResign, &QAction::triggered, this, &MainWindow::GameOver);
    connect(this, &MainWindow::gameEnded, this, &MainWindow::GameOver);

    ui->actionSingle_New_Game->setDisabled(false);
    ui->widgetBoard->setDisabled(true);
    ui->actionResign->setDisabled(true);
}

MainWindow::~MainWindow() {
    delete ui;
    delete board;
}

void MainWindow::SinglePlayerGame() {
    // TODO select color box
    selfColor = Piece_Color::White;
    engine.newGame(selfColor);

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
    case GameState::Win:
        s = QString("You Win");
        break;
    case GameState::Lose:
        s = QString("You Lose");
        break;
    case GameState::Draw:
        s = QString("Draw");
        break;
    // Resign
    default:
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
    CellButton *current_select_btn = getCell(pos);

    // 之前有选中有效的棋子
    if (selectedCell) {
        // 如果当前点的是可行的走位，则移动棋子
        if (highlightCellList.contains(current_select_btn)) {
            Position orig_pos = selectedCell->getPos();
            GameState state = engine.nextGameState(orig_pos, pos);
            updateCellIcon(orig_pos);
            updateCellIcon(pos);

            emit pieceMoved();
            cellCanceled();
            if (state == GameState::Win or state == GameState::Lose or state == GameState::Draw) {
                emit gameEnded();
            }
        } else {
            cellCanceled();
        }
    }
    //
    else {
        Piece *p = engine.getPiece(pos);
        // 如果选中棋子 且 选中了己方的棋子
        if (p and p->getColor() == selfColor) {
            // 获取可行的走位
            QList<Position> l = engine.getPossibleMove(pos);
            if (l.isEmpty()) {
                // 选的棋子没有可行的走位
                selectedCell = nullptr;
            } else {
                selectedCell = current_select_btn;
                foreach (Position pos, l) {
                    // 标为黄色
                    CellButton *btn = getCell(pos);
                    btn->setStyleSheet("background-color: rgba(255,255,0,63);");
                    highlightCellList.append(btn);
                }
            }
        }
    }
}

void MainWindow::cellCanceled() {
    if (selectedCell) {
        foreach (CellButton *btn, highlightCellList) {
            btn->setStyleSheet("background-color: rgba(0,0,0,0);");
        }
        highlightCellList.clear();
        selectedCell = nullptr;
    }
}

void MainWindow::updateCellIcon(Position pos) {
    Piece *p = engine.getPiece(pos);
    if (p) {
        if (p->getColor() == Piece_Color::White) {
            getCell(pos)->setIcon(QIcon(WhiteIcon.value(p->getType(), QString())));
        } else {
            getCell(pos)->setIcon(QIcon(BlackIcon.value(p->getType(), QString())));
        }
    } else {
        getCell(pos)->setIcon(QIcon(QString()));
    }
}

CellButton *MainWindow::getCell(Position pos) {
    return board[convertPosToIndex(pos)];
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
