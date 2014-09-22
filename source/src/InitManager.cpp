#include "InitManager.h"

#include "Algorithm.h"
#include "Global.h"
#include "Game.h"
#include "World.h"

// 4 tipos, 0 sendo a nave mae
#define RANDOM_UNIT (rand()%3 +1)

InitManager::InitManager(int _armyType)
	:armyType(_armyType)
{

}
InitManager::~InitManager(){

}

Army *InitManager::generateRandomArmy()
{
    Army *randomArmy = 0;
    int gold = GOLD_AMOUNT;

    switch(armyType)
    {
        case 0:
            randomArmy = new Army("r", Game::getGlobalGame()->getDictionary("Human"));
        break;

        case 1:
            randomArmy = new Army("r", Game::getGlobalGame()->getDictionary("Elders"));
        break;

        case 2:
            randomArmy = new Army("r", Game::getGlobalGame()->getDictionary("Scrolls"));
        break;
    }

    //Mothership
    randomArmy->createUnit(0, Coordinates(20, Game::getGlobalGame()->getHeight()/2));

    //Create units until run out of money
    while (gold >= 10)
    {
        int randValue = RANDOM_UNIT;

        if(gold - randValue*10 >= 0)
        {
            gold -= randValue*10;
            randomArmy->createUnit(randValue, Coordinates(rand()%TEAM_AREA_WIDTH, rand()%TEAM_AREA_HEIGHT));
        }
    }

    const int nUnits = randomArmy->nUnits();

    //Go through each unit adding tactics
    for(int j = 0; j < nUnits; j++)
    {
        Unit *unit = randomArmy->getUnitAtIndex(j);

        // Gera K taticas
        const int nTacticsToGen = rand()%6+1;
        for(int k = 0; k < nTacticsToGen; k++)
        {
            unit->addTactic( generateRandomTactic(randomArmy, j) );
        }
    }

    return randomArmy;
}

// Static
Tactic* InitManager::generateRandomTactic( const Army* forArmy, int forUnitID )
{
    // Gerar TacticTrigger
    // Gera 2 triggers, seus operadores e valores associados
    TacticTrigger tacticTrig(generateRandomTrigger(),
                             generateRandomTrigger(), rand()%TRIGGER_LOGIC_TOTAL);

    // Gerar tatica valida para nave mae tambem
    int randValue = rand()%TACTIC_NUMBER;
    while ( forUnitID==0 && randValue > 3 )
        randValue = rand()%3;

    switch (randValue)
    {
        case TACTIC_ATTACK_NEAREST_ENEMY:
            return new AttackNearestEnemy(TacticInfo(nullptr), tacticTrig);

        case TACTIC_ATTACK_WEAKEST_ENEMY:
            return new AttackWeakestEnemy(TacticInfo(nullptr), tacticTrig);

        case TACTIC_ATTACK_COLLAB:
            randValue = rand()%forArmy->nUnits();
                while (randValue == forUnitID) randValue = rand()%forArmy->nUnits();
            return new AttackCollab(TacticInfo(forArmy->getUnitAtIndex(randValue)), tacticTrig);

        case TACTIC_DEFENSE_COLLAB:
            randValue = rand()%forArmy->nUnits();
                while (randValue == forUnitID) randValue = rand()%forArmy->nUnits();
            return new DefenseCollab(TacticInfo(forArmy->getUnitAtIndex(randValue)), tacticTrig);

        case TACTIC_KAMIKASE:
            return new Kamikase(TacticInfo(nullptr), tacticTrig);

        case TACTIC_RETREAT:
            randValue = rand()%forArmy->nUnits();
                while (randValue == forUnitID) randValue = rand()%forArmy->nUnits();
            return new Retreat(TacticInfo(forArmy->getUnitAtIndex(randValue)), tacticTrig);
    }

    return nullptr;
}

Trigger* InitManager::generateRandomTrigger()
{
    int randValue = rand()%TRIGGER_TOTAL;

    switch (randValue)
    {
        case TRIGGER_ALWAYS:
            return new Trigger_Always();

        case TRIGGER_LIFE:
            return new Trigger_Life(rand()%100+1, rand()%TRIGGER_OPERATION_TOTAL);
    }

    return nullptr;
}
