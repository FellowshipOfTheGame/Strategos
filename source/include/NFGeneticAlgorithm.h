//
//  GeneticAlgorithm.h
//  Strategos
//
//

#ifndef _NFGENETIC_ALGORITHM_
#define _NFGENETIC_ALGORITHM_

#include "Algorithm.h"
#include "Army.h"
#include "InitManager.h"
#include "CrossoverManager.h"
#include "MutationManager.h"
#include "Objective.h"

class NFGeneticAlgorithm : public Algorithm
{
    public:
        NFGeneticAlgorithm(int _armyType);
        ~NFGeneticAlgorithm();

        void run() override;

        void selection();
        void randomArmies(int size);

    protected:
        void addInitialArmy(Army *army) override;

    private:
        std::vector<Army*> individuos;
        std::vector<Army*> children;

        void PDFS(int start, int end, int *winner);

        void repair(std::vector<Army *>& selected);

        Army* higherFitnessArmy() override;
        std::vector<Army*>& getSelectedArmies() override;

        InitManager *init;
        CrossoverManager *crossover;
        MutationManager *mutation;
        Objective *objective;
};

#endif
