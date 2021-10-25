#include "livingunit.h"

LivingUnit::LivingUnit(QWidget *parent, GameWindow *gameWindow)
    :QLabel(parent), gameWindow(gameWindow)
{
    _picTimer = new QTimer;
    _attackTimer = new QTimer();
    _healthBar = new QProgressBar(parent);
    _healthBar->setMinimum(0);
    _healthBar->setMaximum(100);
    _healthBar->setStyleSheet("QProgressBar{background:rgba(255,255,255,0);} QProgressBar::chunk{border-radius:5px;background:red}");
    _healthBar->setTextVisible(false);
    _healthBar->setValue(100);
    DrawHealthLine();

    connect(this->_picTimer, &QTimer::timeout, this, &LivingUnit::SwithPic);
    connect(this->_attackTimer, &QTimer::timeout, this, &LivingUnit::Attack);
}

void LivingUnit::DrawHealthLine()
{
    _healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    _healthBar->setValue((float)_curHealth/_maxHealth * 100);
    _healthBar->update();
}
