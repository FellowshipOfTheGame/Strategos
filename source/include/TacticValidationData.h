#ifndef TACTIC_VALIDATION_DATA_
#define TACTIC_VALIDATION_DATA_

class Unit;
class CombatData;
class CombatLog;

#include <vector>

struct TacticValidationData
{
    TacticValidationData(CombatData& cd, const std::vector<Unit*>& enemyU, const std::vector<Unit*>& alliedU, CombatLog* log)
        : combatData(cd), enemyUnits(enemyU), alliedUnits(alliedU), validatingUnit(0), armyLog(log)
    {
    }

    CombatData& combatData;
    const std::vector<Unit*>& enemyUnits;
    const std::vector<Unit*>& alliedUnits;

    Unit* validatingUnit;

    CombatLog *armyLog;
};

#endif // TACTIC_VALIDATION_DATA_
