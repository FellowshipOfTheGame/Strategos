#include <Objective.h>
#include <Unit.h>

#include <set>
#include <Game.h>
#include <World.h>

// Limita a qtd de gold que a Army pode ter
// Remove unidades aleatorias - NAO GARANTE CONSISTENCIA das taticas
void Objective::GoldCap(Army *army)
{
	int gold=0, n;
	for (unsigned int i = 1; i < army->nUnits(); i++)
	{
	    Unit *unit = army->getUnitAtIndex(i);
		gold += unit->getType()*10;
	}

	while (gold > GOLD_AMOUNT)
	{
		n = 1+(rand()%(army->nUnits()-2));
		gold -= army->getUnitAtIndex(n)->getType()*10;
		Unit* removed = army->removeUnit(n);
		delete removed; // TODO: Talvez usar isso em outra army?
	}
}

// Garante que uma Army nao possui inconsistencia nas suas taticas
void Objective::rectifyUnit(Army *ind)
{
    const std::vector<Unit*>& units = ind->getUnits();

    // Criar mapa de units dessa Army
    std::set<const Unit*> unitsmap;
    for (unsigned int i = 0; i < units.size(); i++){
        units[i]->setID(i);
        unitsmap.insert( units[i] );
    }

    // Verificar todas as units
    for (unsigned int i = 0; i < units.size(); i++)
    {
        // Rectify AttackCollab and DefenseCollab
        for (unsigned int j = 0; j < units[i]->getTacticSize(); j++)
        {
            Tactic *tactic = units[i]->getTacticAt(j);

            if(tactic->getType() == TACTIC_ATTACK_COLLAB
            || tactic->getType() == TACTIC_DEFENSE_COLAB
            || tactic->getType() == TACTIC_RETREAT) // TODO: Retreat tem ally???
            {
                // If allyUnit is out of range, AttackCollab or DefenseCollab with the Mothership
                // or the reference does not belong to this army, give it a new ally
                if (tactic->getInfo().allyUnit == nullptr
                ||  unitsmap.find( tactic->getInfo().allyUnit ) == unitsmap.end() )
                {
                    const Unit* ally = ind->getUnitAtIndex( rand()%units.size() );
                    tactic->setInfo( TacticInfo(ally) );
                }
            }
        }
    }
}

double Objective::calculateFitness(const Army *ind, int simSteps)
{
    double fitness = 0.0;

    const std::vector<Unit*>& units = ind->getUnits();

    if ( units.size() == 0 ){
        return 0.0;
    }
    Unit *motherUnit = units[0];

    const double max_steps = 900000.0;
    // Perder -> Quanto mais demorar melhor. -0.5 a 0.5
    if(motherUnit->getNShipsAlive() == 0){
        if (simSteps > max_steps)
            return 0.5;
        return -0.5+simSteps/max_steps;
    }else{ // Ganhar: Quanto mais rapido melhor. 0.5 a 1.5
        return 0.5+1.0/(double)simSteps;
    }

//    // Porcentagem de perdas
//    for (unsigned int i = 1; i < units.size(); i++)
//    {
//        countDead += units[i]->nShips()-units[i]->getNShipsAlive();
//        totalShips += units[i]->nShips();
//    }
//    totalShips += units[0]->nShips();
//
//    if (totalShips == 0)
//        fitness = 0;
//    else{
//        fitness = 1.0 - (double)totalShips/((double)countDead+1);
//    }

    return fitness;
}

int Objective::fight(Army *a, Army *b, int *steps)
{
	World *world = new World(a, b, nullptr, nullptr);

    *steps = 0;
    int ret = _SIM_CONTINUE_;
    while(ret == _SIM_CONTINUE_){
        ++(*steps);
        ret = world->simulateStep();
    }
    delete world;
    return *steps;
}
void Objective::evaluate(Army *a, Army *b, double *fitA, double *fitB){
    int steps;
    int ret = this->fight(a, b,&steps);

    double moreFitA = 0;
    double moreFitB = 0;
    if (ret == _SIM_DRAW_){
//                printf("Draw!\n");
        moreFitA = 0.05;
        moreFitB = 0.05;
    }else if (ret == _SIM_ARMY0_WIN_){
//                printf("Winner: %d\n", i);
        moreFitA = 0.15;
    }else{
        moreFitB = 0.15;
//                printf("Winner: %d\n", opponent);
    }

    *fitA = this->calculateFitness(a, steps) + moreFitA;
    *fitB = this->calculateFitness(b, steps) + moreFitB;
}
