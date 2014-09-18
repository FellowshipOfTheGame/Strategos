/*
 *  Strategos - World.cpp
 *
 */

#include "World.h"

#include "Global.h"
#include "Game.h"

using namespace std;

/// Armies iguais = Todas as ships serao as mesmas
/// quando uma morre, ambas morrem. ERRO ERRO

World::World(Army *army1, Army *army2, CombatLog *log1, CombatLog *log2, bool word_type)
    :   combatData(army1->nUnits(), army2->nUnits()),
        army1Log( log1 ), army2Log( log2 ),
        tvdForArmy1(combatData, army2->getUnits(), army1->getUnits(), army1Log),
        tvdForArmy2(combatData, army1->getUnits(), army2->getUnits(), army2Log)
{
    totalSteps = 0;

//    printf("Starting World... ");

    if (army1 == army2)
    {
        printf("ERROR: Armies iguais!\n");
        exit(-99);
    }

    armies.push_back(army1);
	armies.push_back(army2);

//	printf("%d VS %d ", army1->nUnits(), army2->nUnits());

	// Setar posicao de cada exercito
	army1->restore(0, word_type, army1Log);
	army2->restore(1, word_type, army2Log);

//    printf("World Ready!\n");
}

World::~World()
{
//    printNActions();
//    print_MaxActions();
//    printf("Steps: %d\n", totalSteps);
}

void World::calcActions()
{
    // ARMY 0
    for (unsigned int n = 0; n < armies[0]->nUnits(); n++){
        Unit *unit = armies[0]->getUnitAtIndex(n);
        unit->generateActions(tvdForArmy1);
    }

    // ARMY 1
    for (unsigned int n = 0; n < armies[1]->nUnits(); n++){
        Unit *unit = armies[1]->getUnitAtIndex(n);
        unit->generateActions(tvdForArmy2);
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
    ++totalSteps;

    combatData.ClearDistances();

	//printf ("\t calcActions\n");
    calcActions();
  //  printf ("\t\t updateActions 1\n");
    armies[0]->updateActions();
   // printf ("\t\t updateActions 2\n");
    armies[1]->updateActions();
   // printf ("\t\t Update 1 \n");
    int N0 = armies[0]->update(combatData.randomengine);
   // printf ("\t\t Update 2 \n");
    int N1 = armies[1]->update(combatData.randomengine);

    if (army1Log)
        army1Log->nextStep();
    if (army2Log)
        army2Log->nextStep();

    if (N0 == 0 && N1 == 0)
    {
        return _SIM_DRAW_;
    }
    else if (N0 == 0){
        return _SIM_ARMY1_WIN_;
    }
    else if (N1 == 0)
    {
        return _SIM_ARMY0_WIN_;
    }
//    else
//        printf("Sobrando: %d, %d\n", N0, N1);

    return _SIM_CONTINUE_;
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

//    SDL_RenderDrawLine(Game::getGlobalGame()->getRenderer(), TEAM_AREA_WIDTH, 0, TEAM_AREA_WIDTH, TEAM_AREA_HEIGHT);
//    SDL_RenderDrawLine(Game::getGlobalGame()->getRenderer(), TEAM_2_POSX, 0, TEAM_2_POSX, TEAM_AREA_HEIGHT);
}
