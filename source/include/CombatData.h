#ifndef COMBATDATA_H
#define COMBATDATA_H

#include <Unit.h>
#include <Ship.h>

typedef std::pair<const Unit*, const Unit*> p_units;
typedef std::pair<const Ship*, const Ship*> p_ships;

class CombatData
{
    public:
        CombatData();
        ~CombatData();

        double getUnitDistance(const Unit* a, const Unit* b);
        double getShipDistance(const Ship* a, const Ship* b);

        Unit* getNearestUnit(const Unit* from, const std::vector<Unit*>& to);
        Ship* getNearestShip(const Ship* from, const std::vector<Ship*>& to);

        void ClearDistances();

    private:
        std::map<p_units, double> unitDistance;
        std::map<p_ships, double> shipDistance;

};


#endif // COMBATDATA_H
