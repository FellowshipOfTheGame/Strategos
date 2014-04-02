/*
 *  Strategos - World.cpp
 *
 */

#include "World.h"
#include "Global.h"

using namespace std;

/// Armies iguais = Todas as ships serao as mesmas
/// quando uma morre, ambas morrem. ERRO ERRO

World::World(Army *army1, Army *army2)
{
    printf("Starting World... ");

    if (army1 == army2)
    {
        printf("ERROR: Armies iguais!\n");
        exit(-99);
    }

    armies.push_back(army1);
	armies.push_back(army2);

	printf("%d VS %d ", army1->nUnits(), army2->nUnits());

	army1->restore();
	army2->restore();

	// Setar posicao de cada exercito
    armies[1]->setReflectBasePositions(COMBAT_AREA_WIDTH);
    Game::getGlobalGame()->setCombatLog(0);
    Game::getGlobalGame()->setCombatLog(1);
    printf("World Ready!\n");
}

World::~World()
{

}

void World::calcActions()
{
    // ARMY 0
    for (unsigned int n = 0; n < armies[0]->nUnits(); n++){
        Unit *unit = armies[0]->getUnitAtIndex(n);
        unit->generateActions(armies[1]->getUnits(), armies[0]->getUnits());
    }

    // ARMY 1
    for (unsigned int n = 0; n < armies[1]->nUnits(); n++){
        Unit *unit = armies[1]->getUnitAtIndex(n);
        unit->generateActions(armies[0]->getUnits(), armies[1]->getUnits());
    }

    // Aproximar naves mae
    Unit *mother0 = armies[0]->getMotherUnit();
    Unit *mother1 = armies[1]->getMotherUnit();
    Coordinates md1 = mother0->getAveragePos();
    Coordinates md2 = mother1->getAveragePos();
    if ( md1.distance( md2 ) > std::min(mother0->getUnitInfo()->stats.range,
                                        mother1->getUnitInfo()->stats.range) )
    {
        mother0->moveTo( md2 );
        mother1->moveTo( md1 );
    }
}

int World::simulateStep()
{
	//printf ("\t calcActions\n");
    calcActions();
  //  printf ("\t\t updateActions 1\n");
    armies[0]->updateActions();
   // printf ("\t\t updateActions 2\n");
    armies[1]->updateActions();
   // printf ("\t\t Update 1 \n");
    int N0 = armies[0]->update();
   // printf ("\t\t Update 2 \n");
    int N1 = armies[1]->update();

    if (N0 == 0 && N1 == 0)
    {
    	makeLog(armies[0],Game::getGlobalGame()->getCombatLog(0)); // TODO: Mover logs para World
    	makeLog(armies[1],Game::getGlobalGame()->getCombatLog(1));
        return _SIM_DRAW_;
    }
    else if (N0 == 0){
        makeLog(armies[0],Game::getGlobalGame()->getCombatLog(0));
        makeLog(armies[1],Game::getGlobalGame()->getCombatLog(1));
        return _SIM_ARMY1_WIN_;
    }
    else if (N1 == 0)
    {
        makeLog(armies[0],Game::getGlobalGame()->getCombatLog(0));
        makeLog(armies[1],Game::getGlobalGame()->getCombatLog(1));
        return _SIM_ARMY0_WIN_;
    }
//    else
//        printf("Sobrando: %d, %d\n", N0, N1);

    return _SIM_CONTINUE_;
}

void World::makeLog(Army *army, CombatLog *log)
{
	for (unsigned int n = 0; n < army->nUnits(); n++){
        Unit *unit = army->getUnitAtIndex(n);
        for (unsigned int j = 0; j < unit->getUnitInfo()->squadSize; j++){
            Ship *ship = unit->getShip(j);
            if (ship->logUpdate() == 1)
            {
                log->addLog(n, j);
            }
        }
	}
}

void World::printLoadedArmy()
{
    if (armies[0]){
        printf("Army:\n");
        printf("Name: %s\n", armies[0]->getName().c_str());
        printf("Dictionary: %s\n", armies[0]->getDictionary()->title.c_str());
        printf("Units: \n");

        armies[0]->printUnits();
    }
    else
    {
        printf("Army 1 - Not Loaded!\n");
    }
}

Army* World::getArmy(int id)
{
    return armies[id];
}

void World::render()
{
    armies[0]->render();
    armies[1]->render();
}
