#ifndef COMBATDATA_H
#define COMBATDATA_H

#include "Unit.h"
#include "Ship.h"

class CombatData
{
    public:
        CombatData(int nU1, int nU2);
        ~CombatData();

        float getUnitDistance(const Unit* a, const Unit* b);
//        float getShipDistance(const Ship* a, const Ship* b);

        Unit* getNearestUnit(const Unit* from, const std::vector<Unit*>& to, float &outDist);
//        Ship* getNearestShip(const Ship* from, const std::vector<Ship*>& to);

        //Unit* getWeakestUnit(const Unit* from, const std::vector<Unit*>& to);

        void ClearDistances();

        RandomEngine randomengine;

    private:

        int miss, total;

        float* distancesUnitAtoB; //!> [unitA*nUnitsB + unitB]
//        float* distancesUnitAtoA;
//        float* distancesUnitBtoB;
        const int nUnits1, nUnits2;
};


#endif // COMBATDATA_H
