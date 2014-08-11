#include "MutationManager.h"

#include "Unit.h"
#include "InitManager.h"

// 4 tipos, 0 sendo a nave mae
#define RANDOM_UNIT (rand()%3 +1)

MutationManager::MutationManager()
{

}

MutationManager::~MutationManager()
{

}

void MutationManager::mutate(Army *ind){
	mutate(ind, static_cast<ARMY_MUTATION_KIND>(rand()%ARMY_MUTATION_KIND::AMK_UNIT_TOTAL));
}

void MutationManager::mutate(Army *ind, ARMY_MUTATION_KIND degree)
{
	int unitID = rand()%ind->nUnits();
    Unit *unit = ind->getUnitAtIndex(unitID);

    int tacticID = rand()%unit->getTacticSize();
    Tactic *tactic = unit->getTacticAt(tacticID);

    int newType = unit->getType();

    switch(degree)
    {
        case AMK_UNIT_TYPE:
            // Pode causar inconsistencia nas taticas [rectify no final dessa funcao arruma]
            mutateUnitType(ind, unitID, newType);
        break;

        case AMK_UNIT_TACTIC:
            mutateTactic(tactic, static_cast<TACTIC_MUTATION_KIND>(rand()%TMK_MUTATION_TOTAL) );
        break;

        case AMK_UNIT_POSITION:
            unit->setBluePrintCoord( Coordinates(rand()%TEAM_AREA_WIDTH, rand()%TEAM_AREA_HEIGHT ) );
        break;

        case AMK_UNIT_INSERT_NEW:
        {
            int ntactics = rand()%6+1;
            unit = ind->createUnit(RANDOM_UNIT, Coordinates(rand()%TEAM_AREA_WIDTH, rand()%TEAM_AREA_HEIGHT));
            for (int i = 0; i < ntactics; ++i)
            {
                unit->addTactic( InitManager::generateRandomTactic(ind, unit->getID()) );
            }
        }
        break;

        default:
            break;
    }
}

void MutationManager::mutateUnitType(Army* ind, int unitID, int newType)
{
    Unit *removed = ind->getUnitAtIndex(unitID);

    // Do not let mothership's type mutate
    if(removed->getType() == 0)
        return;

    while (newType == removed->getType())
        newType = rand()%(N_UNIT_TYPE-1) + 1;   // Mothership is always 0

    ind->removeUnit(unitID);

    Unit *newUnit = ind->createUnit( newType, removed->getBluePrintCoord() );
    for (unsigned int i = 0; i < removed->getTacticSize(); ++i)
        newUnit->addTactic( Tactic::copy(removed->getTacticAt(i)) );

    delete removed;
}

//Possible mutations
// Unit -> type
// Tactic:
//  -> Trigger type
//  -> Trigger value
//  -> Trigger relational operator
//  -> Triggers logic operator
void MutationManager::mutateTactic(Tactic *tactic, TACTIC_MUTATION_KIND degree)
{
    int newValue;
    int whichTrigger = rand()%2;
    int triggerValue, triggerOperator;

    switch (degree)
    {
        case TMK_TRIGGER_TYPE:
        {
            if (whichTrigger == 0){
                triggerValue = tactic->getTacticTrigger().getTriggerA()->getValue();
                triggerOperator = tactic->getTacticTrigger().getTriggerA()->getRelationalOperator();
            }else{
                triggerValue = tactic->getTacticTrigger().getTriggerB()->getValue();
                triggerOperator = tactic->getTacticTrigger().getTriggerB()->getRelationalOperator();
            }

            Trigger *newtrigger = nullptr;

            newValue = rand()%TRIGGER_TOTAL;
            switch (newValue)
            {
                case TRIGGER_ALWAYS:
                    newtrigger = new Trigger_Always();
                break;

                case TRIGGER_LIFE:
                    newtrigger = new Trigger_Life( triggerValue, triggerOperator );
                break;
            }

            if (whichTrigger == 0)
                tactic->getTacticTrigger().setTriggerA( newtrigger );
            else
                tactic->getTacticTrigger().setTriggerB( newtrigger );
        }
        break;

        case TMK_TRIGGER_VALUE:
            newValue = rand()%100+1;

            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setValue(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setValue(newValue);
        break;

        case TMK_TRIGGER_REL_OP:
            newValue = rand()%TRIGGER_OPERATION_TOTAL;
            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setRelOperator(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setRelOperator(newValue);
        break;

        case TMK_TRIGGER_LOGIC_OP:
            newValue = rand()%TRIGGER_LOGIC_TOTAL;
            tactic->getTacticTrigger().setLogicOperator(newValue);
        break;

        default:
        break;
    }
}
