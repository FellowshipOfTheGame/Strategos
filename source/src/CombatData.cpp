#include "CombatData.h"

CombatData::CombatData(int nU1, int nU2)
    : randomengine(1337), nUnits1(nU1), nUnits2(nU2)
{
    distancesUnitAtoB = new float[nUnits1*nUnits2];

    for (int j = 0; j < nUnits1*nUnits2; ++j)
        distancesUnitAtoB[j] = -1;

    total = miss = 0;
}

CombatData::~CombatData()
{
    delete[] distancesUnitAtoB;
}

float CombatData::getUnitDistance(const Unit* a, const Unit* b)
{
    int x = a->getTeam();
    int y = b->getTeam();

    if (x == y)
    {
        printf("Same Team not expected!\n");
        exit(0);
    }

    if (x == 0)
    {
        x = a->getID();
        y = b->getID();
    }else{
        x = b->getID();
        y = a->getID();
    }

    ++total;
    if ( distancesUnitAtoB[x*nUnits2 + y] < 0 ){
        ++miss;
        return (distancesUnitAtoB[x*nUnits2 + y] = a->getAveragePos().distance2(b->getAveragePos()));
    }

    return distancesUnitAtoB[x*nUnits2 + y];
}

//float CombatData::getShipDistance(const Ship* a, const Ship* b)
//{
//    p_ships s(a, b);
//
//    if ( a > b ){
//        s.first = b;
//        s.second = a;
//    }
//
//    auto it = shipDistance.find(s);
//
//    if (it == shipDistance.end())
//    {
//        float d = a->getPosition().distance(b->getPosition());
//        shipDistance[s] = d;
//        return d;
//    }
//
//    return it->second;
//}

Unit* CombatData::getNearestUnit(const Unit* from, const std::vector<Unit*>& to, float &outDist)
{
    Unit *nearestUnit = nullptr;
    outDist = 999999;

	for (unsigned int i = 0; i < to.size(); ++i)
	{
	    if (to[i]->getNShipsAlive() == 0) continue;

        float dist = getUnitDistance(from, to[i]);
        if (dist < outDist)
        {
            outDist = dist;
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
//    printf("Total: %d, Miss: %d, TaxSuc: %.3f\n", total, miss, 1.0-miss/(float)total);
//    total = miss = 0;
    for (int j = 0; j < nUnits1*nUnits2; ++j)
        distancesUnitAtoB[j] = -1;
}
