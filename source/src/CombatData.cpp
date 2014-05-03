#include "CombatData.h"

CombatData::CombatData(int nU1, int nU2)
    : randomengine(1337), nUnits1(nU1), nUnits2(nU2)
{
    distancesUnit = new double*[nU1];

    for (int i = 0; i < nU1; ++i){
        distancesUnit[i] = new double[nU2];

        for (int j = 0; j < nU2; ++j)
            distancesUnit[i][j] = -1;
    }

    total = miss = 0;
}

CombatData::~CombatData()
{
    for (int i = 0; i < nUnits1; ++i)
        delete[] distancesUnit[i];
    delete[] distancesUnit;
}

double CombatData::getUnitDistance(const Unit* a, const Unit* b)
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
    if ( distancesUnit[x][y] < 0 ){
        ++miss;
        return (distancesUnit[x][y] = a->getAveragePos().distance(b->getAveragePos()));
    }

    return distancesUnit[x][y];
}

//double CombatData::getShipDistance(const Ship* a, const Ship* b)
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
//        double d = a->getPosition().distance(b->getPosition());
//        shipDistance[s] = d;
//        return d;
//    }
//
//    return it->second;
//}

Unit* CombatData::getNearestUnit(const Unit* from, const std::vector<Unit*>& to, double &outDist)
{
    Unit *nearestUnit = nullptr;
    outDist = 999999;

	for (unsigned int i = 0; i < to.size(); ++i)
	{
	    if (to[i]->getNShipsAlive() == 0) continue;

        double dist = getUnitDistance(from, to[i]);
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
//    printf("Total: %d, Miss: %d, TaxSuc: %.3lf\n", total, miss, 1.0-miss/(double)total);
//    total = miss = 0;
    for (int i = 0; i < nUnits1; ++i)
    {
//        for (int j = 0; j < nUnits2; ++j)
//            distancesUnit[i][j] = -1;
        memset( distancesUnit[i], -5, nUnits2*sizeof(double) );
    }
}
