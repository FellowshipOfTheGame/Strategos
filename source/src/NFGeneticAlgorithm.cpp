//
//  NFGeneticAlgorithm.cpp
//  Strategos
//
//

#include <sstream>
#include <set>

#include <thread>

#include "NFGeneticAlgorithm.h"

#include "Global.h"
#include "Game.h"
#include "World.h"



//#define _DEBUG_

static std::mutex printMutex;
static std::mutex populationMutex;
#define printTH(x...){ printMutex.lock(); printf(x); printMutex.unlock(); }

const double mutation_chance = 0.3;
const int num_geracoes = 2;


#define INDIVIDUOS_GERACAO 16


NFGeneticAlgorithm::NFGeneticAlgorithm(int _armyType)
    : Algorithm(_armyType)
{

    this->init = new InitManager(_armyType);
    this->crossover = new CrossoverManager();
    this->mutation = new MutationManager();
    this->objective = new Objective();
}

NFGeneticAlgorithm::~NFGeneticAlgorithm()
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


void NFGeneticAlgorithm::randomArmies(int size)
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


void NFGeneticAlgorithm::addInitialArmy(Army *army)
{
    individuos.push_back(army);

}

void NFGeneticAlgorithm::repair(std::vector<Army *> selected){
    for(std::vector<Army *>::iterator it = selected.begin(); it != selected.end(); it++){
        this->objective->GoldCap(*it);
        this->objective->rectifyUnit(*it);
    }
}

void NFGeneticAlgorithm::PDFS(int start, int end, int *winner){
    int mid = (start + end) / 2;
    if(start == end) {
        *winner = start;
        return;
    }
    int win1, win2;
    //PDFS(start, mid, &win1);
    std::thread t = std::thread(&NFGeneticAlgorithm::PDFS,this, start,mid, &win1);
    PDFS(mid+1, end, &win2);
    t.join();
    double fitA, fitB;
    this->individuos[win1]->Lock();
    Army *armyA = Army::clone(this->individuos[win1]);
    this->individuos[win1]->Unlock();
    this->individuos[win2]->Lock();
    Army *armyB = Army::clone(this->individuos[win2]);
    this->individuos[win2]->Unlock();

    Army *child1, *child2;
    if(end - start > 2){
        this->crossover->crossOver(armyA,armyB, child1, child2);
        this->mutation->mutate(child1);
        this->mutation->mutate(child2);
        populationMutex.lock();
        children.push_back(child1);
        children.push_back(child2);
        populationMutex.unlock();
    }
    int ret = this->objective->evaluate(armyA, armyB, &fitA, &fitB);
    delete armyA;
    delete armyB;
    *winner = (ret == 1) ? win1 : win2;

}

void NFGeneticAlgorithm::run()
{
    srand(time(nullptr));
    for (unsigned int i = 0; i < num_geracoes; i++)
    {
        printf("GENERATION %d\n", i);
        // Completar exercito para INDIVIDUOS_GERACAO
        randomArmies( INDIVIDUOS_GERACAO - individuos.size());
        repair(individuos);
        int elite;
        PDFS(0, INDIVIDUOS_GERACAO-1, &elite);
        children.push_back(Army::clone(individuos[elite]));
        //deletar
        for(Army *a: individuos)
        {
            delete a;
        }
        individuos.clear();
        for(Army *a: children)
        {
            individuos.push_back(a);
        }
        children.clear();
    }
    repair(individuos);
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
std::vector<Army*>& NFGeneticAlgorithm::getSelectedArmies()
{
    return individuos;
}

Army* NFGeneticAlgorithm::higherFitnessArmy()
{
    printf("Selecting higherFitnessArmy\n");

    std::string name = "GA/"+std::to_string(armyType)+"/"+(*individuos.rbegin())->getName();
    printf("Selected: %s\n", name.c_str());

    return Army::loadArmy(name);
}
