//
//  GeneticAlgorithm.cpp
//  Strategos
//
//

#include <sstream>
#include <set>

#include <thread>

#include "GeneticAlgorithm.h"

#include "Global.h"
#include "Game.h"
#include "World.h"

#define MUTATION_UNIT_TYPE      0
#define MUTATION_UNIT_TACTIC    1
#define MUTATION_UNIT_POSITION  2
#define MUTATION_ADD_UNIT       3
#define MUTATION_TOTAL          4

// 4 tipos, 0 sendo a nave mae
#define RANDOM_UNIT (rand()%3 +1)

//#define _DEBUG_

static std::mutex printMutex;
#define printTH(x...){ printMutex.lock(); printf(x); printMutex.unlock(); }

const double mutation_chance = 0.3;
const int add_random_armies = 2;
const int num_geracoes = 50;




// Static
Tactic* GeneticAlgorithm::generateRandomTactic( const Army* forArmy, int forUnitID )
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

        case TACTIC_DEFENSE_COLAB:
            randValue = rand()%forArmy->nUnits();
                while (randValue == forUnitID) randValue = rand()%forArmy->nUnits();
            return new DefenseCollab(TacticInfo(forArmy->getUnitAtIndex(randValue)), tacticTrig);

        case TACTIC_KAMIKASE:
            return new Kamikase(TacticInfo(nullptr), tacticTrig);

        case TACTIC_RETREAT:
            return new Retreat(TacticInfo(nullptr), tacticTrig);
    }

    return nullptr;
}

Trigger* GeneticAlgorithm::generateRandomTrigger()
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


GeneticAlgorithm::GeneticAlgorithm(int _armyType)
    : Algorithm(_armyType)
{

    allowedThreads = std::thread::hardware_concurrency();
    this->crossover = new CrossoverManager();
    this->objective = new Objective();
}

GeneticAlgorithm::~GeneticAlgorithm()
{
    for (unsigned int i = 0; i < individuos.size(); ++i)
    {
        delete individuos[i];
    }
    delete this->crossover;
    delete this->objective;
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
#ifdef _DEBUG_
        printf("Generating %d... ", i);
#endif

        armyName.assign("r");
        armyName.append(std::to_string(i));

        Army *randomArmy = generateRandomArmy();
        randomArmy->setArmyName(armyName);

        //Save created army
        individuos.push_back(randomArmy);
    }

}

Army* GeneticAlgorithm::generateRandomArmy()
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

    this->objective->rectifyUnit(randomArmy);

    return randomArmy;
}
void GeneticAlgorithm::addInitialArmy(Army *army)
{
    individuos.push_back(army);

}

void GeneticAlgorithm::repair(std::vector<Army *> selected){
    for(std::vector<Army *>::iterator it = selected.begin(); it != selected.end(); it++){
        this->objective->GoldCap(*it);
        this->objective->rectifyUnit(*it);
    }
}

void GeneticAlgorithm::run()
{
    for (unsigned int i = 0; i < num_geracoes; i++)
    {
        std::vector<Army*> selected, rejected;

//#ifdef _DEBUG_
        printf("GENERATION %d\n", i);
//#endif

        srand(time(nullptr));

        // Completar exercito para INDIVIDUOS_GERACAO
        randomArmies( INDIVIDUOS_GERACAO - individuos.size());
        // Adicionar alguns individuos aleatorios
        randomArmies( add_random_armies );

        // Apply the selection - currently Tournament
        selectFromPop(SELECT_FROM_POP, selected, rejected);

        for (unsigned int r = 0; r < rejected.size(); ++r){
            delete rejected[r];
        }
        individuos.clear();
        rejected.clear();

        Army* best = Army::clone(selected[0]);

        // Apply CrossOver
        crossOver(selected);

        // Apply Mutation
        mutate(selected);

        //Repair children
        repair(selected);

        individuos.push_back(best);
        for (unsigned int j = 0; j < selected.size(); ++j){
            individuos.push_back(selected[j]);
        }
        selected.clear();
    }

    char buffer[8];

    //Save the strongest armies
#ifdef _DEBUG_
    printf("Salvando Individuos... %d\n", individuos.size());
#endif
    for (unsigned int i = 0; i < individuos.size(); i++)
    {
        saveArmy(individuos[i], i);
    }
}

void GeneticAlgorithm::selectFromPop(unsigned int n, std::vector<Army*>& selected, std::vector<Army*>& rejected)
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

    printf("TOP 5:\n");
    for (int i = 0; i < 5; ++i){
        printf("Fit[%d] = %lf [%d]\n", i, individuos[i]->getFitness(), individuos[i]->nUnits());
    }

//    printf("Selecting and Rejecting...");

    // Selecionar os N primeiros
    for (unsigned int i = 0; i < individuos.size(); ++i){
        if (i < n)
            selected.push_back(individuos[i]);
        else
            rejected.push_back(individuos[i]);
    }
}

void GeneticAlgorithm::threadSimulate( unsigned int from, unsigned int n )
{
    printMutex.lock();
    std::cout << "Thread Start: " << std::this_thread::get_id() ;
    printf(" - %d %d!\n", from, n);
    printMutex.unlock();

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

            

            Army* armyB = individuos[opponent];
            armyB->Lock();
            Army* armyBclone = Army::clone( armyB );
            armyB->Unlock();


#ifdef _DEBUG_
//Sequencial
            printf("->Battle: %d with %d -- Units: %d vs %d\n", i, opponent, individuos[i]->nUnits(), individuos[opponent]->nUnits());
#endif
            int steps;
            int ret = this->objective->fight(armyAclone, armyBclone,&steps);

            // Bonus para ganhadores
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

            fitA += this->objective->calculateFitness(armyAclone, steps) + moreFitA;
            const double fitB = this->objective->calculateFitness(armyBclone, steps) + moreFitB;
            delete armyBclone;

            // Utilizar essa batalha para o exercito B tambem, mas com peso menor.
            armyB->Lock();
                if (armyB->getFitness() == DONT_HAVE_FITNESS){
                    armyB->setFitness(fitB);
//                    printf("%d FitB: %lf\n", opponent, fitB);
                }else{
                    armyB->setFitness(armyB->getFitness()*0.8 + fitB*0.2);
                }
			armyB->Unlock();
        }
        fitA /= nBattlesToFit;

        armyA->Lock();
            if (armyA->getFitness() == DONT_HAVE_FITNESS){
                armyA->setFitness(fitA);
//                printf("%d FitA: %lf\n", i, fitA);
            }else{
                armyA->setFitness(armyA->getFitness()*0.7 + fitA*0.3);
            }
        armyA->Unlock();
        delete armyAclone;
    }

    printMutex.lock();
    std::cout << "Thread Finished: " << std::this_thread::get_id();
    printf(" - %d %d!\n", from, n);
    printMutex.unlock();
}

void GeneticAlgorithm::crossOver(std::vector<Army*>& selected)
{
    std::vector<Army*> indCross;

//    printf("crossover for: %d\n", selected.size());
    for (unsigned int i = 0; i < selected.size(); ++i)
    {
        unsigned int other = rand()%selected.size();
        while ( other == i )
            other = rand()%selected.size();

        this->crossover->crossOver(selected[i], selected[other], indCross);
    }



//    printf("CrossOvers: %d [%d]\n", indCross.size(), indCross.capacity());

    // Deletar alguns individuos extras aleatoriamente
    while (indCross.size() > 0 && (indCross.size() + selected.size()) > INDIVIDUOS_GERACAO)
    {
        auto it = indCross.begin()+ (rand()%indCross.size());
        delete *it;
        indCross.erase( it );
    }

//    printf("CrossOvers: %d\n", indCross.size());
    for (unsigned int i = 0; i < indCross.size(); ++i){
        selected.push_back(indCross[i]);
    }
}

void GeneticAlgorithm::mutate(std::vector<Army*>& selected)
{
    for (unsigned int i = 0; i < selected.size(); ++i)
    {
        if ( (double)rand()/(double)RAND_MAX < mutation_chance)
            mutation(selected[i], rand()%MUTATION_TOTAL);
    }
}





std::vector<Army*>& GeneticAlgorithm::getSelectedArmies()
{
    return individuos;
}

Army* GeneticAlgorithm::higherFitnessArmy()
{
    printf("Selecting higherFitnessArmy\n");
    std::vector<Army*> best, rejected;

    selectFromPop(1, best, rejected);

    std::string name = "GA/"+std::to_string(armyType)+"/"+best[0]->getName();
    printf("Selected: %s\n", name.c_str());

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
            // Pode causar inconsistencia nas taticas [rectify no final dessa funcao arruma]
            mutateUnitType(ind, unitID, newType);
        }
        break;

        case MUTATION_UNIT_TACTIC: //Mutate a unit tactic
            mutateTactic(tactic, rand()%4);
        break;

        case MUTATION_UNIT_POSITION: //Mutate a unit position
            unit->setBluePrintCoord( Coordinates(rand()%TEAM_AREA_WIDTH, rand()%TEAM_AREA_HEIGHT ) );
        break;

        case MUTATION_ADD_UNIT:
        {
            int ntactics = rand()%6+1;
            unit = ind->createUnit(RANDOM_UNIT, Coordinates(rand()%TEAM_AREA_WIDTH, rand()%TEAM_AREA_HEIGHT));
            for (int i = 0; i < ntactics; ++i)
            {
                unit->addTactic( generateRandomTactic(ind, unit->getID()) );
            }
        }
        break;

        default:
            break;
    }
}

void GeneticAlgorithm::mutateUnitType(Army* ind, int unitID, int newType)
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
void GeneticAlgorithm::mutateTactic(Tactic *tactic, int degree)
{
    int newValue;
    int whichTrigger = rand()%2;
    int triggerValue, triggerOperator;

    switch (degree)
    {
        case 0: //  -> Trigger type
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
