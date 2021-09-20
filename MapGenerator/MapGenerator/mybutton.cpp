#include "mybutton.h"

MyButton::MyButton(QWidget *parent, int r, int c) : QPushButton(parent), r(r), c(c)
{
    connect(this, &QPushButton::clicked, this, [=](){emit mouseClicked(this);});
}
