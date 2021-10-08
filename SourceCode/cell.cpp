#include "cell.h"
#include "ui_cell.h"

Cell::Cell(int type, int resourceType, QWidget* parent, int r, int c)
    :QLabel(parent), ui(new Ui::Cell), _resourceType(resourceType), _r(r), _c(c)
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
    this->_type = type;
    if(type & Path)
        _cellType = Path;
    else if(type & Blocked)
        _cellType = Blocked;
    else if(type & Placable)
        _cellType = Placable;
}
