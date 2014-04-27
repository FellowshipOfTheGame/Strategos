//
//  GeneticAlgorithm.cpp
//  Strategos
//
//

#include <dirent.h>
#include <time.h>
#include <sstream>

#include <thread>


#include "GeneticAlgorithm.h"
#include "World.h"
#include "Global.h"

#define MUTATION_UNIT_TYPE      0
#define MUTATION_UNIT_TACTIC    1
#define MUTATION_UNIT_POSITION  2
#define MUTATION_TOTAL          3

GeneticAlgorithm::GeneticAlgorithm(int _armyType)
    : Algorithm(), armyType(_armyType)
{
    directory = "GA/"+std::to_string(armyType)+"/";

    allowedThreads = std::thread::hardware_concurrency();
}

GeneticAlgorithm::~GeneticAlgorithm()
{
    for (unsigned int i = 0; i < individuos.size(); ++i)
    {
        delete individuos[i];
    }
}

void GeneticAlgorithm::initialize()
{
    DIR *dir;
    dirent *ent;

    std::string dirpath = "assets/saves/GA/" + std::to_string(armyType) + "/";

    //Carrega exercitos prontos (tirar o -blah para funcionar)
    if ((dir = opendir(dirpath.c_str())) != nullptr)
    {
        while ((ent = readdir (dir)) != nullptr)
        {
            if(ent->d_name[0] == 'r')
            {
                std::string tmp = "GA/" + std::to_string(armyType) + "/";

                tmp += ent->d_name;
                tmp.replace(tmp.length()-4, 4, "\0");

                Army *army = Army::loadArmy(tmp);

                if(army != nullptr)
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

    srand((unsigned)time(nullptr));

}

void GeneticAlgorithm::randomArmies(int size)
{
    if (size <= 0) return;

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
        randomArmy->setArmyName(armyName);

        //Save created army
        individuos.push_back(randomArmy);

//        Army::saveArmy(randomArmy, directory.c_str());
    }

}

Army* GeneticAlgorithm::generateRandomArmy()
{
    Trigger* triggers[2];
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

        // Gera K taticas
        for(int k = 0; k < 4; k++)
        {
            // Gera 2 triggers, seus operadores e valores associados
            for(int l = 0; l < 2; l++)
            {
                randValue = rand()%TRIGGER_TOTAL;

                switch (randValue)
                {
                    case TRIGGER_ALWAYS:
                        triggers[l] = new Trigger_Always();
                    break;

                    case TRIGGER_LIFE:
                        triggers[l] = new Trigger_Life(rand()%100+1, rand()%3);
                    break;
                }
            }

           TacticTrigger tacticTrig(triggers[0], triggers[1], rand()%2);

            //Gera a tatica
            Tactic *tactic=0;
            randValue = rand()%TACTIC_NUMBER;
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
        }
    }

    return randomArmy;
}

void GeneticAlgorithm::run()
{
    unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
    printf("Threads max support: %d\n", concurentThreadsSupported);

    if (armyType != 0) return; // debug

    printf("Iterate 2 times\n");
    for (unsigned int i = 0; i < 10; i++)
    {
        std::vector<Army*> selected, rejected;

        printf("GENERATION %d\n", i);
        // Completar exercito para INDIVIDUOS_GERACAO
        // Adicionar 2 aleatorios sempre
        randomArmies( INDIVIDUOS_GERACAO - individuos.size() + 2 );

        // Apply the selection - currently Tournament
        selectFromPop(SELECT_FROM_POP, selected, rejected);

        for (unsigned int r = 0; r < rejected.size(); ++r){
            delete rejected[r];
        }
        individuos.clear();
        rejected.clear();

        // Apply CrossOver
        crossOver(selected);

        // Apply Mutation
        mutate(selected);

        printf("Iteration Individuals: %d\n", selected.size());
        for (unsigned int j = 0; j < selected.size(); ++j){
            individuos.push_back(selected[j]);
        }
        selected.clear();
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

//    exit(0);
}

void GeneticAlgorithm::selectFromPop(int n, std::vector<Army*>& selected, std::vector<Army*>& rejected)
{
    // Executar batalhas para calcular o fitnes
    printf("Battle for %d individuos\n", individuos.size());

    std::vector<std::thread> threads;

    const int nPerThread = individuos.size()/allowedThreads;
    for (int i = 0; i < allowedThreads; ++i){
        threads.push_back( std::thread(&GeneticAlgorithm::threadSimulate, this, i*nPerThread, nPerThread ) );
    }

    for (int i = 0; i < allowedThreads; ++i){
        threads[i].join();
    }

    // Ordenar
    // TODO: Fazer algo melhor que bubble sort =)
    for (unsigned int i = 0; i < individuos.size(); ++i)
    {   for (unsigned int j = 0; j < individuos.size(); ++j)
        {
            if (individuos[i]->getFitness() > individuos[j]->getFitness())
            {
                Army* tmp = individuos[i];
                individuos[i] = individuos[j];
                individuos[j] = tmp;
            }
        }
    }

    printf("TOP 10:\n");
    for (int i = 0; i < 10; ++i){
        printf("Fit[%d] = %lf [%d]\n", i, individuos[i]->getFitness(), individuos[i]->nUnits());
    }

    printf("Selecting and Rejecting...");

    // Selecionar os N primeiros
    for (unsigned int i = 0; i < individuos.size(); ++i){
        if (i < n)
            selected.push_back(individuos[i]);
        else
            rejected.push_back(individuos[i]);
    }

    if (n == 1)
    {
        printf("BEST: %lf\n", individuos[0]->getFitness());
    }
}

void GeneticAlgorithm::threadSimulate( int from, int n )
{
    for (unsigned int i = from; i < from+n; ++i)
    {
        int nBattlesToFit = 1;
        double fitA = 0;

        Army* armyA = individuos[i];
        armyA->Lock();
        Army* armyAclone = Army::clone( armyA );
        armyA->Unlock();

        for (int b = 0; b < nBattlesToFit; ++b)
        {
            unsigned int opponent = rand()%individuos.size();
            while (opponent == i)
                opponent = rand()%individuos.size();

            int ret = _SIM_CONTINUE_;

            Army* armyB = individuos[opponent];
            armyB->Lock();
            Army* armyBclone = Army::clone( armyB );
            armyB->Unlock();

            //Sequencial

            printf("->Battle: %d with %d -- Units: %d vs %d\n", i, opponent, individuos[i]->nUnits(), individuos[opponent]->nUnits());
            World *world = new World(armyAclone, armyBclone);

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

            fitA += evaluateFitness(armyAclone) + moreFit;
            const double fitB = evaluateFitness(armyBclone);
            delete armyBclone;

            // Utilizar essa batalha para o exercito B tambem, mas com peso menor.
            armyB->Lock();
            	armyB->setFitness(armyB->getFitness()*0.8 + fitB*0.2);
			armyB->Unlock();
        }
        fitA /= nBattlesToFit;

        armyA->Lock();
            armyA->setFitness(armyA->getFitness()*0.7 + fitA*0.3);
        armyA->Unlock();
        delete armyAclone;
    }
}

void GeneticAlgorithm::crossOver(std::vector<Army*>& selected)
{
    std::vector<Army*> indCross;

    printf("crossover for: %d\n", selected.size());
    for (unsigned int i = 0; i < selected.size(); ++i)
    {
        unsigned int other = rand()%selected.size();
        while ( other == i )
            other = rand()%selected.size();

        crossOver(selected[i], selected[other], indCross); // Memory Leak
    }

    printf("CrossOvers: %d [%d]\n", indCross.size(), indCross.capacity());

    // Deletar alguns individuos extras aleatoriamente
    while (indCross.size() > 0 && (indCross.size() + selected.size()) > INDIVIDUOS_GERACAO)
    {
        auto it = indCross.begin()+ (rand()%indCross.size());
        delete *it;
        indCross.erase( it );
    }

    printf("CrossOvers: %d\n", indCross.size());
    for (unsigned int i = 0; i < indCross.size(); ++i){
        selected.push_back(indCross[i]);
    }
}

void GeneticAlgorithm::mutate(std::vector<Army*>& selected)
{
    for (unsigned int i = 0; i < selected.size(); ++i)
    {
        if ( (double)rand()/(double)RAND_MAX < 0.2)
            mutation(selected[i], rand()%MUTATION_TOTAL);
    }
}


//Crossover of Armies with same race/dictionary
//Because armies can have different sizes (in units), we will use "Cut and Splice"
void GeneticAlgorithm::crossOver(const Army *parent1, const Army *parent2, std::vector<Army*>& ind)
{
    std::string str;
    str.assign(parent1->getName().c_str());
    str.append(parent2->getName().c_str());
    Army* child1 = new Army(str, parent1->getDictionary());

    str.assign(parent2->getName().c_str());
    str.append(parent1->getName().c_str());
    Army* child2 = new Army(str, parent2->getDictionary());

    unsigned int i;
    for (i = 0; i < parent1->nUnits() && i < parent2->nUnits(); ++i )
    {
        if (rand()%2 == 0)
        {
            child1->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
            child2->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
        }
        else
        {
            child1->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
            child2->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
        }
    }

    // Colocar as restantes nos respectivos filhos 1 e 2
    // Apenas um desses 2 loops deve ocorrer, por isso a mesma variavel i eh usada
    for (; i < parent1->nUnits(); ++i){
        child1->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
    }
    for (; i < parent2->nUnits(); ++i){
        child2->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
    }

    GoldCap(child1);
    GoldCap(child2);

    rectifyUnitID(child1);
    rectifyUnitID(child2);

    //Save the new armies
    ind.push_back(child1);
    ind.push_back(child2);
}

double GeneticAlgorithm::evaluateFitness(const Army *ind)
{
    double fitness = 0.0;
    int countDead = 0, totalShips = 0;

    const std::vector<Unit*>& units = ind->getUnits();

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

//    printf("GoldCap: %d -> ", army->nUnits());
	while (gold > GOLD_AMOUNT)
	{
		n = 1+(rand()%(army->nUnits()-2));
		gold -= army->getUnitAtIndex(n)->getType()*10;
		Unit* removed = army->removeUnit(n);
		delete removed; // TODO: Talvez usar isso em outra army?
	}
//    printf("%d\n", army->nUnits());
}

void GeneticAlgorithm::rectifyUnitID(Army *ind)
{
    std::vector<Unit*> *units = ind->getUnitsReference();

    for (unsigned int i = 0; i < units->size(); i++)
    {
        units->at(i)->setID(i);

        //Rectify AttackCollab and DefenseCollab
        for (unsigned int j = 0; j < units->at(i)->getTacticSize(); j++)
        {
            Tactic *tactic = units->at(i)->getTacticAt(j);

            if(tactic->getType() == TACTIC_ATTACK_COLLAB
            || tactic->getType() == TACTIC_DEFENSE_COLAB
            || tactic->getType() == TACTIC_RETREAT) // TODO: Retreat tem ally???
            {
                //If allyUnit is out of range, AttackCollab or DefenseCollab with the Mothership
                if(tactic->getInfo().allyUnitID >= units->size())
                    tactic->setInfo(TacticInfo(0));
            }
        }
    }
}

std::vector<Army*>& GeneticAlgorithm::getSelectedArmies()
{
    return individuos;
}

Army* GeneticAlgorithm::higherFitnessArmy()
{
    std::vector<Army*> best, rejected;

    selectFromPop(1, best, rejected);

    std::string name = "/GA/"+std::to_string(armyType)+"/"+best[0]->getName();

    return Army::loadArmy(name);
}


void GeneticAlgorithm::mutation(Army *ind, int degree)
{
    int unitID = rand()%ind->nUnits();
    Unit *unit = ind->getUnitAtIndex(unitID);

    int tacticID = rand()%unit->getTacticSize();
    Tactic *tactic = unit->getTacticAt(tacticID);

    int newType = unit->getType();

    switch(degree)
    {
        case MUTATION_UNIT_TYPE:{ //Mutate a unit type
            mutateUnitType(ind, unitID, newType);
        }
        break;

        case MUTATION_UNIT_TACTIC: //Mutate a unit tactic
            mutateTactic(tactic, rand()%4);
        break;

        case MUTATION_UNIT_POSITION: //Mutate a unit position
            unit->setBluePrintCoord( Coordinates(rand()%TEAM_AREA_WIDTH + 25, rand()%TEAM_AREA_HEIGHT ) );
        break;

        default:
            break;
    }
    GoldCap(ind);
}

void GeneticAlgorithm::mutateUnitType(Army* ind, int unitID, int newType)
{
    Unit *removed = ind->getUnitAtIndex(unitID);

    if(removed->getType() == 0)    //Do not let mothership's type mutate
        return;

    while(newType == removed->getType())
        newType = rand()%(N_UNIT_TYPE-1) + 1;   //Mothership is always 0

    ind->removeUnit(unitID);
    Unit *newUnit = ind->createUnit(removed->getID(), newType, removed->getBluePrintCoord());
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
void GeneticAlgorithm::mutateTactic(Tactic *tactic, int degree)
{
    int newValue;
    int whichTrigger = rand()%2;
    int triggerValue, triggerOperator;

    switch (degree)
    {
        case 0: //  -> Trigger type
        {
            newValue = rand()%TRIGGER_TOTAL;

            if (whichTrigger == 0){
                triggerValue = tactic->getTacticTrigger().getTriggerA()->getValue();
                triggerOperator = tactic->getTacticTrigger().getTriggerA()->getRelationalOperator();
            }else{
                triggerValue = tactic->getTacticTrigger().getTriggerB()->getValue();
                triggerOperator = tactic->getTacticTrigger().getTriggerB()->getRelationalOperator();
            }

            switch (newValue)
            {
                case TRIGGER_ALWAYS:
                    if (whichTrigger == 0)
                        tactic->getTacticTrigger().setTriggerA( new Trigger_Always() );
                    else
                        tactic->getTacticTrigger().setTriggerB( new Trigger_Always() );
                break;

                case TRIGGER_LIFE:
                    if (whichTrigger == 0)
                        tactic->getTacticTrigger().setTriggerA( new Trigger_Life( triggerValue, triggerOperator ) );
                    else
                        tactic->getTacticTrigger().setTriggerB( new Trigger_Life( triggerValue, triggerOperator ) );
                break;
            }
        }
        break;

        case 1: //  -> Trigger value
            newValue = rand()%100+1;

            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setValue(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setValue(newValue);
        break;

        case 2: //  -> Trigger relational operator
            newValue = rand()%TRIGGER_OPERATION_TOTAL;
            if(whichTrigger == 0)
                tactic->getTacticTrigger().getTriggerA()->setRelOperator(newValue);
            else
                tactic->getTacticTrigger().getTriggerB()->setRelOperator(newValue);
        break;

        case 3: //  -> Triggers logic operator
            newValue = rand()%TRIGGER_LOGIC_TOTAL;
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
