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




//#define _DEBUG_

static std::mutex printMutex;
#define printTH(x...){ printMutex.lock(); printf(x); printMutex.unlock(); }

const double mutation_chance = 0.3;
const int add_random_armies = 2;
const int num_geracoes = 50;


GeneticAlgorithm::GeneticAlgorithm(int _armyType)
    : Algorithm(_armyType)
{

    allowedThreads = std::thread::hardware_concurrency();
    this->init = new InitManager(_armyType);
    this->crossover = new CrossoverManager();
    this->mutation = new MutationManager();
    this->objective = new Objective();
}

GeneticAlgorithm::~GeneticAlgorithm()
{
    for (unsigned int i = 0; i < individuos.size(); ++i)
    {
        delete individuos[i];
    }
    delete this->init;
    delete this->crossover;
    delete this->mutation;
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

        Army *randomArmy = this->init->generateRandomArmy();
        randomArmy->setArmyName(armyName);

        //Save created army
        individuos.push_back(randomArmy);
    }

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
            double _fitA, _fitB;
            this->objective->evaluate(armyAclone, armyBclone, &_fitA, &_fitB);
            delete armyBclone;

            fitA += _fitA;

            // Utilizar essa batalha para o exercito B tambem, mas com peso menor.
            armyB->Lock();
                if (armyB->getFitness() == DONT_HAVE_FITNESS){
                    armyB->setFitness(_fitB);
//                    printf("%d FitB: %lf\n", opponent, fitB);
                }else{
                    armyB->setFitness(armyB->getFitness()*0.8 + _fitB*0.2);
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
            this->mutation->mutate(selected[i]);
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