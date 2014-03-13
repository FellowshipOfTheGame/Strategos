#include "Ship.h"
#include "Global.h"
#include "Random.h"

using namespace std;

#define M_PI 3.141592653

float slowlyRotateTo(float fromRad, float toRad)
{
	return toRad-fromRad;

	if (fabs(toRad-fromRad) < M_PI)
        return toRad-fromRad;
    if (toRad>fromRad)
        return toRad-fromRad-M_PI;//*2.0f;
    return toRad-fromRad+M_PI*2.0f;
}

Ship::Ship(const shipBaseStats &initialStats, Coordinates Coord, int shipID)
    : baseStats(initialStats), stats(initialStats), coord(Coord), basePos(Coord)
{
    //printf ("shiled: %3d hp:%3d \n",stats.currentShield, stats.currentHP);
    this->id = shipID;

    direction = M_PI/2;

    moving = 0;
    status = 0;
    deathTime=0;
}

Ship::~Ship()
{

}

const Coordinates& Ship::getPosition()
{
    return coord;
}

const Coordinates& Ship::getTargetPos()
{
    return targetPos;
}

float Ship::getDirection()
{
    return direction;
}

int Ship::update()
{
    if (stats.currentAtkCD > 0)
        --(stats.currentAtkCD);

    if (stats.curKamikazeCD > 0)
        --(stats.curKamikazeCD);

    if (!moving) //So dar uma nova coordenada quando 'terminar' o movimento atual
    {
        moving = 1;
        targetPos.x = basePos.x+rand()%30-rand()%30;
        targetPos.y = basePos.y+rand()%30-rand()%30;
    }

    direction += slowlyRotateTo( direction, coord.angleTo(targetPos) )/15;

    double distance = coord.distance(targetPos);

    if(distance > baseStats.speed)
    {
        coord.x -= ((coord.x-targetPos.x)/distance)*baseStats.speed/1.5;
        coord.y -= ((coord.y-targetPos.y)/distance)*baseStats.speed/1.5;
    }
    else
    {
        moving = 0;
    }

    return 0;
}

bool Ship::isAlive()
{
    if(stats.currentHP > 0)
        return true;

    return false;
}

void Ship::kill()
{
	stats.currentHP = -1;
}

const shipBaseStats &Ship::getBaseStats()
{
    return baseStats;
}

shipCurrentStats &Ship::getStats()
{
    return stats;
}

bool Ship::takeDamage(double damage)
{
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

void Ship::move(Coordinates delta)
{
    coord += delta;
    basePos += delta;

    moving = 0;
}

void Ship::move(double dx, double dy)
{
    basePos.x += dx;
    basePos.y += dy;

    coord.x += dx;
    coord.y += dy;

    moving = 0;
}

void Ship::moveTo(Coordinates c)
{
    targetPos = c;
    basePos = c;

    moving = 1;
}

double Ship::getHP(){
    return stats.currentHP;
}

unsigned long Ship::getID(){
    return id;
}

int Ship::getX(){
    return coord.x;
}

int Ship::getY(){
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
