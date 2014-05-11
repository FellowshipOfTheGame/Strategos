
#include "Ship.h"

#include "CombatLog.h"
#include "Global.h"

using namespace std;

int directionTo(float fromRad, float toRad)
{
    float dir = (fromRad - toRad); // * 180.0/M_PI;

	if (dir > 0 && fabs(dir) <= M_PI) return -1;
	else if (dir > 0 && fabs(dir) > M_PI) return 1;
	else if (dir < 0 && fabs(dir) <= M_PI) return 1;
	else if (dir < 0 && fabs(dir) > M_PI) return -1;

    return 0;
}

Ship::Ship(const shipBaseStats &initialStats, const Coordinates& Coord, CombatRound* log)
    : baseStats(initialStats), stats(initialStats),
      coord(Coord), unitPos(Coord), targetPos(Coord), myLog(log)
{
    currentDirection = M_PI/2;

    status = 0;
}

Ship::~Ship()
{
}

const Coordinates& Ship::getPosition() const
{
    return coord;
}

const Coordinates& Ship::getTargetPos() const
{
    return targetPos;
}

float Ship::getDirection() const
{
    return currentDirection;
}

int Ship::update(RandomEngine& randE)
{
    if (stats.currentAtkCD > 0)
        --(stats.currentAtkCD);

    if (stats.curKamikazeCD > 0)
        --(stats.curKamikazeCD);

    // So dar uma nova coordenada quando 'terminar' o movimento atual
    // Evitar que as naves fiquem totalmente paradas
    if (stats.isMoving == move_not_moving)
    {
        targetPos.x = unitPos.x+ randE.nextInt()%int(2*SPACIAL_UNIT)-randE.nextInt()%int(2*SPACIAL_UNIT);
        targetPos.y = unitPos.y+ randE.nextInt()%int(2*SPACIAL_UNIT)-randE.nextInt()%int(2*SPACIAL_UNIT);
        stats.isMoving = move_flying;
    }

    const float destDir = coord.angleTo(targetPos);
    const float angularVelocity = 5.0 * M_PI/180.0; // rad/frame
    const int sign = directionTo( currentDirection, destDir );

    double distance = coord.distance(targetPos);

    if (distance > 0.8*SPACIAL_UNIT)
    {
        // Mover diretamente para o alvo
//        coord.x -= ((coord.x-targetPos.x)/distance)*stats.currentSpeed;
//        coord.y -= ((coord.y-targetPos.y)/distance)*stats.currentSpeed;

        float angledif = fabs(currentDirection - destDir);
        if (angledif > M_PI)
            angledif = 2.0*M_PI - angledif;

        // Dependendo da situacao a nave pode ficar em loop [girando em torno de um centro]
        if ( angledif > angularVelocity )
            currentDirection += angularVelocity*sign;
        else
            currentDirection = destDir;

        coord.x += cos(currentDirection)*stats.currentSpeed;
        coord.y -= sin(currentDirection)*stats.currentSpeed;
    }
    else
    {
        stats.isMoving = move_not_moving;
    }

    return 0;
}

bool Ship::isAlive() const
{
    return (stats.currentHP > 0);
}

void Ship::kill()
{
    if (stats.currentHP <= 0) return;

	stats.currentHP = -1;

	if (myLog)
    {
        myLog->addLog( RoundData(0, 0, 1, 0) );
    }
}

const shipBaseStats &Ship::getBaseStats() const
{
    return baseStats;
}

shipCurrentStats &Ship::getStats()
{
    return stats;
}

bool Ship::takeDamage(double damage)
{
    RoundData rd(damage, 0,0,0);

    //printf ("shiled: %3d hp:%3d \n",stats.currentShield, stats.currentHP);
    if (stats.currentShield > 0)
    {
        stats.currentShield -= damage;
        if (stats.currentShield < 0)
            damage = -stats.currentShield;
        else
            damage = 0.0;
    }

    stats.currentHP -= damage;

    if (myLog){
        rd.deaths = (stats.currentHP <= 0);
        myLog->addLog(rd);
    }

    return (stats.currentHP <= 0);
}

bool Ship::dealDamage(double damage, bool wasLetal)
{
    if (myLog)
    {
        myLog->addLog( RoundData(0, damage, 0, wasLetal) );
    }
}

void Ship::moveTo(const Coordinates& c)
{
    targetPos = c;
    unitPos = c;

    stats.isMoving = move_action;
}

double Ship::getHP() const{
    return stats.currentHP;
}

int Ship::getX() const{
    return coord.x;
}

int Ship::getY() const{
    return coord.y;
}
