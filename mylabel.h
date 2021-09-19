#ifndef MYLABEL_H
#define MYLABEL_H

#include "header.h"

namespace Ui {
class MyLabel;
}

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MyLabel(QWidget *parent = nullptr);
    ~MyLabel();

private:
    Ui::MyLabel *ui;
};

#endif // MYLABEL_H
