#include "enemy.h"

Enemy::Enemy(double health, double attack, int dealHealthDamage, double rangedDamageRate, \
        double closeDamageRate, double attackInterval, QPropertyAnimation* attackAnimation\
        , QPropertyAnimation* moveAnimation)
    :maxHealth(health), curHealth(health), attack(attack), dealHealthDamage(dealHealthDamage),
      rangedDamageRate(rangedDamageRate), closeDamageRate(closeDamageRate), attackInterval(attackInterval), \
      attackAnimation(attackAnimation), moveAnimation(moveAnimation)
{

}
