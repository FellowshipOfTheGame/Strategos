//
//  GeneticAlgorithm.cpp
//  Strategos
//
//

#include <dirent.h>
#include <time.h>
#include <sstream>
#include "GeneticAlgorithm.h"
#include "World.h"
#include "Global.h"


GeneticAlgorithm::~GeneticAlgorithm()
{

}

void GeneticAlgorithm::initialize()
{
    DIR *dir;
    dirent *ent;

    std::string dirpath = "assets/saves/GA/" + std::to_string(armyType) + "/";

    //Carrega exercitos prontos (tirar o -blah para funcionar)
    if ((dir = opendir(dirpath.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if(ent->d_name[0] == 'r')
            {
                string tmp = "GA/" + std::to_string(armyType) + "/";

                tmp += ent->d_name;
                tmp.replace(tmp.length()-4, 4, "\0");

                Army *army = Army::loadArmy(tmp);

                if(army != NULL)
                    individuos.push_back(army);
            }
        }

        closedir (dir);
    }
    else
    {
        printf("Could not open directory\n");
        mkdir(dirpath.c_str());
    }

    srand((unsigned)time(NULL));
    //srand48((long)time(NULL));

}

void GeneticAlgorithm::randomArmies(int size)
{
    std::ostringstream stream;
    std::string armyName;

    //TODO: Atualizar os precos, etc.
    //Apenas para teste usando base:
    // - 100 illenium disponivel
    // - Mothership ja inclusa
    // - Scout Type: $10
    // - Balanced Type: $20
    // - Tanker Type: $30

    // Criar o maximo de naves possivel pois a quantidade de illenium gasta ainda nao esta entrando no fitness
    for(int i = 1; i <= size; i++)
    {
        printf("Generating %d... ", i);

        armyName.assign("r");
        armyName.append(std::to_string(i));

        Army *randomArmy = generateRandomArmy();
        randomArmy->setArmyName(armyName.c_str());

        //Save created army
        individuos.push_back(randomArmy);

//        Army::saveArmy(randomArmy, directory.c_str());
    }

}

Army* GeneticAlgorithm::generateRandomArmy()
{
    vector<Trigger*> triggers;
    Army *randomArmy = 0;
    int u = 0;
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
    randomArmy->createUnit(0, 0, Coordinates(20, Game::getGlobalGame()->getHeight()/2));

    //Create units until run out of money
    while (gold >= 10)
    {
        int randValue = rand()%3 + 1;

        if(gold - randValue*10 >= 0)
        {
            gold -= randValue*10;
            randomArmy->createUnit(++u, randValue, Coordinates(rand()%800, rand()%Game::getGlobalGame()->getHeight()%600));
        }
    }

    int nUnits = randomArmy->nUnits();
    int randValue;

    //Go through each unit adding tactics
    for(int j = 0; j < nUnits; j++)
    {
        Unit *unit = randomArmy->getUnitAtIndex(j);

        // Gera 20 taticas
        for(int k = 0; k < 4; k++)
        {
            // Gera 2 triggers, seus operadores e valores associados
            for(int l = 0; l < 2; l++)
            {
                Trigger *trig;
                randValue = rand()%N_TRIGGERS;

                switch (randValue)
                {
                    case TRIGGER_ALWAYS:
                        trig = new Trigger_Always();
                        break;

                    case TRIGGER_LIFE:
                        trig = new Trigger_Life(rand()%100+1, rand()%3);
                        break;
                }

                triggers.push_back(trig);
            }

           TacticTrigger tacticTrig(triggers[0], triggers[1], rand()%2);

            //Gera a tatica
            Tactic *tactic=0;
            randValue = rand()%N_RULES;
            switch (randValue)
            {
                case 0:
                    tactic = new AttackNearestEnemy(TacticInfo(0), tacticTrig);
                    break;
                case 1:
                    tactic = new AttackWeakestEnemy(TacticInfo(0), tacticTrig);
                    break;
                case 2:
                    tactic = new AttackCollab(TacticInfo(rand()%nUnits), tacticTrig);
                    break;
                case 3:
                    tactic = new DefenseCollab(TacticInfo(rand()%nUnits), tacticTrig);
                    break;
                case 4:
                    tactic = new Kamikase(TacticInfo(0), tacticTrig);
                    break;
                case 5:
                    tactic = new Retreat(TacticInfo(0), tacticTrig);
                    break;
            }

            if ((unit->getType()==0 && (randValue<2)) || (unit->getType()!=0))
            {
                 unit->addTactic(tactic);
            }
            else
            {
                unit->addTactic(new AttackNearestEnemy(TacticInfo(0), tacticTrig));
                delete tactic;
             }

            triggers.clear();
        }
    }

    return randomArmy;
}

void GeneticAlgorithm::run()
{
    if(individuos.size() == 0)
        randomArmies(INDIVIDUOS_GERACAO);

    if (armyType != 0) return;

    printf("Iterate 2 times\n");
    for (unsigned int i = 0; i < 20; i++)
    {
        vector<Army*> selected, rejected;

        // Completar exercito
        randomArmies( INDIVIDUOS_GERACAO - individuos.size() );

        // Apply the selection - currently Tournament
        selectFromPop(20, selected, rejected);

        for (int r = 0; r < rejected.size(); ++r){
            delete rejected[r];
        }
        rejected.clear();

        // Apply CrossOver
        crossOver(selected);

        // Apply Mutation
//        mutate(selected);

        individuos.clear();
        printf("Iteration: %d\n", selected.size());
        for (unsigned int j = 0; j < selected.size(); ++j){
            selected[j]->restore();
            individuos.push_back(selected[j]);
        }
    }

    char buffer[8];

    //Save the strongest armies
    printf("Salvando Individuos... %d\n", individuos.size());
    for (unsigned int i = 0; i < individuos.size(); i++)
    {
        sprintf(buffer, "r%d", i+1);
        individuos[i]->setArmyName(buffer);
        Army::saveArmy(individuos[i], directory.c_str());
        printf("army[%d]\n", i+1);
    }
}

void GeneticAlgorithm::selectFromPop(int n, vector<Army*>& selected, vector<Army*>& rejected)
{
    vector<PairAF> order;

//    for (int i = 0; i < individuos.size(); ++i)
//        printf("%p\n", individuos[i]);

    // Executar batalhas para calcular o fitnes
    printf("Battle for %d individuos\n", individuos.size());
    for (unsigned int i = 0; i < individuos.size(); ++i)
    {
        int nBattlesToFit = 2;
        double fit = 0;
        for (int b = 0; b < nBattlesToFit; ++b)
        {
            unsigned int opponent = rand()%individuos.size();
            while (opponent == i)
                opponent = rand()%individuos.size();

            int ret = _SIM_CONTINUE_;

            //Sequencial
            printf("\nBattle: %d with %d -- Units: %d vs %d\n", i, opponent, individuos[i]->nUnits(), individuos[opponent]->nUnits());
            World *world = new World(individuos[i], individuos[opponent]);

            while(ret == _SIM_CONTINUE_){
                ret = world->simulateStep();
            }

            double moreFit = 0;
            if (ret == _SIM_DRAW_){
                printf("Draw!\n");
                moreFit = 0.05;
            }else if (ret == _SIM_ARMY0_WIN_){
                printf("Winner: %d\n", i);
                moreFit = 0.1;
            }else{printf("Winner: %d\n", opponent);}

            delete world;

            fit += evaluateFitness(individuos[i]) + moreFit;
        }
        fit /= nBattlesToFit;

        // Criar lista
        order.push_back( PairAF(individuos[i], fit) );
    }

    // Ordenar
    // TODO: Fazer algo melhor que bubble sort =)
    for (unsigned int i = 0; i < order.size(); ++i)
    {   for (unsigned int j = 0; j < order.size(); ++j)
        {
            if (order[i].fitness > order[j].fitness)
            {
                PairAF tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }

    printf("TOP 5:\n");
    for (int i = 0; i < 5; ++i){
        printf("Fit[%d] = %lf [%d]\n", i, order[i].fitness, order[i].ind->nUnits());
    }

    // Selecionar os N primeiros
    for (int i = 0; i < order.size(); ++i){
        if (i < n)
            selected.push_back(order[i].ind);
        else
            rejected.push_back(order[i].ind);
    }

    if (n == 1)
    {
        printf("BEST: %lf\n", order[0].fitness);
    }
}

void GeneticAlgorithm::crossOver(vector<Army*>& selected)
{
    vector<Army*> indCross;

    for (int i = 0; i < selected.size(); ++i)
    {
        int other = rand()%selected.size();
        while ( other == i )
            other = rand()%selected.size();

        crossOver(selected[i], selected[other], indCross);
    }

    printf("CrossOvers: %d\n", indCross.size());

    while (indCross.size() + selected.size() > INDIVIDUOS_GERACAO)
    {
        indCross.erase( indCross.begin()+ (rand()%indCross.size()) );
    }

    printf("CrossOvers: %d\n", indCross.size());
    for (int i = 0; i < indCross.size(); ++i)
    {
        selected.push_back(indCross[i]);
    }
}

void GeneticAlgorithm::mutate(vector<Army*>& selected)
{
    for (int i = 0; i < selected.size(); ++i)
    {
        if ( (double)rand()/(double)RAND_MAX < 0.2)
            mutation(selected[i], rand()%3 + 1);
    }
}


//Crossover of Armies with same race/dictionary
//Because armies can have different sizes (in units), we will use "Cut and Splice"
void GeneticAlgorithm::crossOver(const Army *parent1, const Army *parent2, vector<Army*>& ind)
{
    string str;
    str.assign(parent1->getName().c_str());
    str.append(parent2->getName().c_str());
    Army* child1 = new Army(str, parent1->getDictionary());

    str.assign(parent2->getName().c_str());
    str.append(parent1->getName().c_str());
    Army* child2 = new Army(str, parent2->getDictionary());

    int i;
    for (i = 0; i < parent1->nUnits() && i < parent2->nUnits(); ++i )
    {
        if (rand()%2 == 0)
        {
            Unit *unit = parent1->getUnitAtIndex(i);
            child1->addUnit( new Unit(unit) );

            unit = parent2->getUnitAtIndex(i);
            child2->addUnit( new Unit(unit) );
        }
        else
        {
            Unit *unit = parent1->getUnitAtIndex(i);
            child2->addUnit( new Unit(unit) );

            unit = parent2->getUnitAtIndex(i);
            child1->addUnit( new Unit(unit) );
        }
    }

    // Colocar as restantes nos respectivos filhos 1 e 2
    // Apenas um desses 2 loops deve ocorrer, por isso a mesma variavel i eh usada
    for (; i < parent1->nUnits(); ++i)
    {
        Unit *unit = parent1->getUnitAtIndex(i);
        child1->addUnit( new Unit(unit) );
    }
    for (; i < parent2->nUnits(); ++i)
    {
        Unit *unit = parent2->getUnitAtIndex(i);
        child2->addUnit( new Unit(unit) );
    }

    printf("preParents: %d %d [%d] -> %d %d [%d]\n", parent1->nUnits(), parent2->nUnits(), parent1->nUnits()+parent2->nUnits(),
                                                  child1->nUnits(), child2->nUnits(), child1->nUnits()+child2->nUnits());
    GoldCap(child1);
    GoldCap(child2);
    printf("postParents: %d %d [%d]\n", child1->nUnits(), child2->nUnits(), child1->nUnits()+child2->nUnits());
//    SDL_Delay(3000);

    rectifyUnitID(child1);
    rectifyUnitID(child2);

    //Save the new armies
    ind.push_back(child1);
    ind.push_back(child2);

//    Army *child1, *child2 = nullptr;
//    int point1 = rand()%parent1->nUnits()+1;
//    int point2 = rand()%parent2->nUnits()+1;
//    string str;
//
//    //Child #1
//    str.assign(parent1->getName().c_str());
//    str.append(parent2->getName().c_str());
//    child1 = new Army(str, parent1->getDictionary());
//
//    printf("Creating childs\np1 %d, p2 %d\n", point1, point2);
//    printf("Pa1 %d\n", parent1->nUnits());
//    printf("Pa2 %d\n", parent2->nUnits());
//
//    for (int i = 0; i < point1; i++)
//    {
//        Unit *unit = new Unit( parent1->getUnitAtIndex(i) );
//
//        //child1->createUnit(unit1->getID(), unit1->getType(), unit1->getCoord());
//        child1->addUnit( unit );
//    }
//
//    for (unsigned int i = point2; i < parent2->nUnits(); i++)
//    {
//        Unit *unit = new Unit( parent2->getUnitAtIndex(i) );
//
//        //child1->createUnit(unit2->getID(), unit2->getType(), unit2->getCoord());
//        child1->addUnit( unit );
//    }
//
//    //Child #2
//    str.assign(parent2->getName().c_str());
//    str.append(parent1->getName().c_str());
//    child2 = new Army(str, parent2->getDictionary());
//
//    for(int i = 0; i < point2; i++)
//    {
//        Unit *unit = parent2->getUnitAtIndex(i);
//
//        //child2->createUnit(unit2->getID(), unit2->getType(), unit2->getCoord());
//        child2->addUnit( new Unit(unit) );
//    }
//
//    for (unsigned int i = point1; i < parent1->nUnits(); i++)
//    {
//        Unit *unit = parent1->getUnitAtIndex(i);
//
//        //child2->createUnit(unit1->getID(), unit1->getType(), unit1->getCoord());
//        child2->addUnit( new Unit(unit) );
//    }
//
//    printf("preParents: %d %d [%d] -> %d %d [%d]\n", parent1->nUnits(), parent2->nUnits(), parent1->nUnits()+parent2->nUnits(),
//                                                  child1->nUnits(), child2->nUnits(), child1->nUnits()+child2->nUnits());
//
//    GoldCap(child1);
//    GoldCap(child2);
//    printf("postParents: %d %d [%d] -> %d %d [%d]\n", parent1->nUnits(), parent2->nUnits(), parent1->nUnits()+parent2->nUnits(),
//                                                  child1->nUnits(), child2->nUnits(), child1->nUnits()+child2->nUnits());
//    SDL_Delay(3000);
//
//    rectifyUnitID(child1);
//    rectifyUnitID(child2);
//
//    //Save the new armies
//    ind.push_back(child1);
//    ind.push_back(child2);
}

double GeneticAlgorithm::evaluateFitness(const Army *ind)
{
    double fitness = 0.0;
    int countDead = 0, totalShips = 0;

    const vector<Unit*>& units = ind->getUnits();

    if ( units.size() == 0 ){
        return 0.0;
    }
    else
    {
//        printf("Exercito %d\n", units.size());
//        for (int i = 0; i < units.size(); ++i)
//            printf("%d - %p\n", i, units[i]);
    }

    Unit *motherUnit = units[0];

    // O peso da mothership eh maior
    if(motherUnit->getNShipsAlive() == 0)
        countDead += ind->getTotalShips()*5;

    // Porcentagem de perdas
    for (unsigned int i = 1; i < units.size(); i++)
    {
        countDead += units[i]->nShips()-units[i]->getNShipsAlive();
        totalShips += units[i]->nShips();
    }
    totalShips += units[0]->nShips();

    if (totalShips == 0)
        fitness = 0;
    else{
        fitness = 1.0 - (double)totalShips/((double)countDead+1);
    }

    return fitness;
}

//limita a qtd de gold que os filhos terão
void GeneticAlgorithm::GoldCap(Army *army)
{
	int gold=0, n;
	for (unsigned int i = 1; i < army->nUnits(); i++)
	{
	    Unit *unit = army->getUnitAtIndex(i);
		gold += unit->getType()*10;
	}

    printf("GoldCap: %d -> ", army->nUnits());
	while (gold > GOLD_AMOUNT)
	{
		n = 1+(rand()%(army->nUnits()-2));
		gold -= army->getUnitAtIndex(n)->getType()*10;
		army->removeUnit(n);
	}
    printf("%d\n", army->nUnits());
}

void GeneticAlgorithm::rectifyUnitID(Army *ind)
{
    vector<Unit*> *units = ind->getUnitsReference();

    for (unsigned int i = 0; i < units->size(); i++)
    {
        units->at(i)->setID(i);

        //Rectify AttackCollab and DefenseCollab
        for (unsigned int j = 0; j < units->at(i)->getTacticSize(); j++)
        {
            Tactic *tactic = units->at(i)->getTacticAt(j);

            if(tactic->getType() == 2 || tactic->getType() == 3)
            {
                //If allyUnit is out of range, AttackCollab or DefenseCollab with the Mothership
                if(tactic->getInfo().allyUnitID >= units->size())
                    tactic->setInfo(TacticInfo(0));
            }
        }
    }
}

vector<Army*>& GeneticAlgorithm::getSelectedArmies()
{
    return individuos;
}

Army* GeneticAlgorithm::higherFitnessArmy()
{
    std::vector<Army*> best, rejected;

    selectFromPop(1, best, rejected);

    string name = "/GA/"+std::to_string(armyType)+"/"+best[0]->getName();

    return Army::loadArmy(name);
}


//Possible mutations
// Unit -> type
// Tactic:
//  -> Type
//  -> Trigger type
//  -> Trigger value
//  -> Trigger relational operator
//  -> Triggers logic operator
void GeneticAlgorithm::mutation(Army *ind, int degree)
{
    int unitID = rand()%ind->nUnits();
    Unit *unit = ind->getUnitAtIndex(unitID);

    int tacticID = rand()%unit->getTacticSize();
    Tactic *tactic = unit->getTacticAt(tacticID);

    int newType = unit->getType();
    int tacticDegree;

    switch(degree)
    {
        case 1: //Mutate a unit type
            if(unit->getType() == 0)    //Do not let mothership's type mutate
                break;

            while(newType == unit->getType())
                newType = rand()%(N_UNIT_TYPE-1) + 1;   //Mothership is always 0

            unit->setType(newType);
            break;

        case 2: //Mutate a unit tactic
        	do
        	{
            tacticDegree = rand()%5;//+1;
        	}while ((unit->getType()==0) && (tacticDegree>1));
            mutateTactic(tactic, tacticDegree);
            break;

        case 3: //Mutate a unit type & tactic
            if(unit->getType() == 0)    //Do not let mothership's type mutate
                break;

            while (newType == unit->getType())
                newType = rand()%(N_UNIT_TYPE-1) + 1;   //Mothership is always 0

            unit->setType(newType);

            do
			{
			tacticDegree = rand()%5;//+1;
			}while ((unit->getType()==0) && (tacticDegree>1));
            mutateTactic(tactic, tacticDegree);
            break;

        default:
            break;
    }
    GoldCap(ind);
}

void GeneticAlgorithm::mutateTactic(Tactic *tactic, int degree)
{
    int newValue;
    int whichTrigger = rand()%2;

    switch (degree)
    {
        case 1: //  -> Type
            newValue = rand()%N_RULES;
            tactic->setType(newValue);
            break;

        case 2: //  -> Trigger type
            newValue = rand()%N_TRIGGERS;

            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setType(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setType(newValue);
            break;

        case 3: //  -> Trigger value
            newValue = rand()%100+1;

            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setValue(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setValue(newValue);
            break;

        case 4: //  -> Trigger relational operator
            newValue = rand()%6;
            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setRelOperator(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setRelOperator(newValue);
            break;

        case 5: //  -> Triggers logic operator
            newValue = rand()%2;
            tactic->getTacticTrigger().setLogicOperator(newValue);
            break;

        default:
            break;
    }
}

/*
// Tournament Selection
void GeneticAlgorithm::selection()
{
    printf("Selection...\n");
//    vector<PairAF> order;

    //Run the tournament
    //armies.size() have to be even
    for (unsigned int i = 0; i+1 < armies.size(); i+=2)
    {
        int ret = _SIM_CONTINUE_;
        //Sequencial
        printf("Battle: %d with %d\n", i, i+1 );
        printf("Units: %d vs %d\n", armies[i]->nUnits(), armies[i+1]->nUnits() );
        World *world = new World(armies[i], armies[i+1]);

        while(ret == _SIM_CONTINUE_){
            ret = world->simulateStep();
        }

        delete world;

        //Thread Concurrency

        //OMP parallel
    }

    printf("_B.1_");

    //Verificar se as naves voltam com o status
    //Calculate the fitness and assemble a map
    for (unsigned int i = 0; i < armies.size(); i++)
    {
        double fitness = evaluateFitness(armies[i]);
        order.push_back(PairAF(armies[i], fitness));
    }

    printf("_B.2_");

    //Take the best half of the army
    int half = armies.size()/2;

    //printf("half: %d\n", half);

    //Make sure it is an even number
    if(half % 2 != 0)
        half++;

    //It is currently applying elitism with chance of mutation in between generations
    for(int i = 0; i < half; i+=2)
    {
        double chance0 = (double)rand()/(double)RAND_MAX;		//crossover
        double chance1 = (double)rand()/(double)RAND_MAX;       //mudar tipo
        double chance2 = (double)rand()/(double)RAND_MAX;		//mudar tatica

        if(armies[i]->getDictionary() == armies[i+1]->getDictionary() && chance0 <= 0.25)
            crossOver(armies[i], armies[i+1]);

        if(chance1 <= 0.25)
            mutation(armies[i], rand()%3+1);
        if(chance2 <= 0.25)
            mutation(armies[i+1], rand()%3+1);

        selected.push_back(armies[i]);
        selected.push_back(armies[i+1]);
    }

    //Roulette-wheel selection
    //{
        //Normalize fitness
        //normalizeFitness(&order);
        //Sort pairs from highest to lowest
        //std::sort(order.begin(), order.end(), GeneticAlgorithm::highToLow);
    //}
}

//Nao esta sendo usada no momento
void GeneticAlgorithm::normalizeFitness(vector<PairAF> *pairs)
{
    double sum = 0;

    for (unsigned int i = 0; i < pairs->size(); i++)
    {
        sum += pairs->at(i).fitness;
    }

    for (unsigned int i = 0; i < pairs->size(); i++)
    {
        pairs->at(i).fitness /= sum;
    }
}

*/
