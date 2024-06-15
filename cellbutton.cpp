#include "cellbutton.h"

CellButton::CellButton(Position pos) {
    this->pos = pos;
}

void CellButton::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        emit rightClicked(pos);
    } else if (e->button() == Qt::LeftButton) {
        emit leftClicked(pos);
    }
    QPushButton::mousePressEvent(e);
}
