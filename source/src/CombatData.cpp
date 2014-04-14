#include "CombatData.h"

CombatData::CombatData()
{
}

CombatData::~CombatData()
{
}

double CombatData::getUnitDistance(const Unit* a, const Unit* b)
{
    p_units u(a, b);

    if ( a > b ){
        u.first = b;
        u.second = a;
    }

    auto it = unitDistance.find(u);

    if (it == unitDistance.end())
    {
        double d = a->getAveragePos().distance(b->getAveragePos());
        unitDistance[u] = d;
        return d;
    }

    return it->second;
}

double CombatData::getShipDistance(const Ship* a, const Ship* b)
{
    p_ships s(a, b);

    if ( a > b ){
        s.first = b;
        s.second = a;
    }

    auto it = shipDistance.find(s);

    if (it == shipDistance.end())
    {
        double d = a->getPosition().distance(b->getPosition());
        shipDistance[s] = d;
        return d;
    }

    return it->second;
}

Unit* CombatData::getNearestUnit(const Unit* from, const std::vector<Unit*>& to)
{
    Unit *nearestUnit = NULL;
    double minDist = 99999;

	for (unsigned int i = 0; i < to.size(); ++i)
	{
	    if (to[i]->getNShipsAlive() == 0) continue;

        double dist = getUnitDistance(from, to[i]);
        if (dist < minDist)
        {
            minDist = dist;
            nearestUnit = to[i];
        }
	}

	return nearestUnit;
}

//Ship* CombatData::getNearestShip(const Ship* from, const std::vector<Ship*>& to)
//{
//
//}

void CombatData::ClearDistances()
{
    unitDistance.clear();
    shipDistance.clear();
}
