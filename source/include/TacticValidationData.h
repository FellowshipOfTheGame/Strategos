#ifndef TACTIC_VALIDATION_DATA_
#define TACTIC_VALIDATION_DATA_

class Unit;
class CombatData;

struct TacticValidationData
{
    TacticValidationData(CombatData& cd, const vector<Unit*>& enemyU, const vector<Unit*>& alliedU)
        : combatData(cd), enemyUnits(enemyU), alliedUnits(alliedU), validatingUnit(0)
    {
    }

    CombatData& combatData;
    const vector<Unit*>& enemyUnits;
    const vector<Unit*>& alliedUnits;

    Unit* validatingUnit;
};

#endif // TACTIC_VALIDATION_DATA_
