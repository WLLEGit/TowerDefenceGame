#include "cell.h"
#include "ui_cell.h"

Cell::Cell(int type, int resourceType, QWidget* parent, int r, int c)
    :QLabel(parent), ui(new Ui::Cell), resourceType(resourceType), r(r), c(c)
{
    SetType(type);
}

void Cell::mousePressEvent(QMouseEvent *ev)
{
    emit CellPressed(this);
    QLabel::mousePressEvent(ev);
}

void Cell::SetType(int type)
{
    this->type = type;
    if(type & Path)
        cellType = Path;
    else if(type & Blocked)
        cellType = Blocked;
    else if(type & Placable)
        cellType = Placable;
}
