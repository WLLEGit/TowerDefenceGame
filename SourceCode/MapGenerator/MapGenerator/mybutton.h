#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QObject>
#include<QPushButton>

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyButton(QWidget *parent = nullptr, int r=-1, int c=-1);
    int r, c;

signals:
    void mouseClicked(MyButton*);

};

#endif // MYBUTTON_H
