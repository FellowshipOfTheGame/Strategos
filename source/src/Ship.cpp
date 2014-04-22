#include "Ship.h"
#include "Global.h"
#include "Random.h"

using namespace std;

float slowlyRotateTo(float fromRad, float toRad)
{
	return toRad-fromRad;

	if (fabs(toRad-fromRad) < M_PI)
        return toRad-fromRad;
    if (toRad>fromRad)
        return toRad-fromRad-M_PI;//*2.0f;
    return toRad-fromRad+M_PI*2.0f;
}

Ship::Ship(const shipBaseStats &initialStats, Coordinates Coord)
    : baseStats(initialStats), stats(initialStats),
      coord(Coord), unitPos(Coord), targetPos(Coord)
{
    currentDirection = M_PI/2;

    status = 0;
    deathTime=0;
    dmgperround = new CombatRound();
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

int Ship::update()
{
    if (stats.currentAtkCD > 0)
        --(stats.currentAtkCD);

    if (stats.curKamikazeCD > 0)
        --(stats.curKamikazeCD);

    if (stats.isMoving == move_not_moving) //So dar uma nova coordenada quando 'terminar' o movimento atual
    {
        targetPos.x = unitPos.x+rand()%30-rand()%30;
        targetPos.y = unitPos.y+rand()%30-rand()%30;
        stats.isMoving = move_flying;
    }

    currentDirection += slowlyRotateTo( currentDirection, coord.angleTo(targetPos) )/15;

    double distance = coord.distance(targetPos);

    if (distance > stats.currentSpeed)
    {
        coord.x -= ((coord.x-targetPos.x)/distance)*stats.currentSpeed;
        coord.y -= ((coord.y-targetPos.y)/distance)*stats.currentSpeed;
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
	stats.currentHP = -1;
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
    //registra quanto de dano bruto foi recebido
	CombatRoundItem *temp = new CombatRoundItem(this->deathTime, damage);
	dmgperround->addLog(temp);
	delete temp;
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

    return (stats.currentHP <= 0);
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

int Ship::logUpdate()
{
	if (this->stats.currentHP>0)
	{
		deathTime++;
	}
	else
		if (status == 0)
		{
			status = 1;
		}
		else
			status = 2;
	return status;
}
