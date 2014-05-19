//
//  GeneticAlgorithm.h
//  Strategos
//
//

#ifndef _NFGENETIC_ALGORITHM_
#define _NFGENETIC_ALGORITHM_

#include <stdio.h>

#include <Algorithm.h>
#include <Army.h>
#include <InitManager.h>
#include <CrossoverManager.h>
#include <MutationManager.h>
#include <Objective.h>

class NFGeneticAlgorithm : public Algorithm
{
    public:
        NFGeneticAlgorithm(int _armyType);
        ~NFGeneticAlgorithm();
        void run() override;

        void selection();
        void randomArmies(int size);

    protected:
        virtual void addInitialArmy(Army *army);
    private:
        std::vector<Army*> individuos;
        std::vector<Army*> children;

        void PDFS(int start, int end, int *winner);

        void repair(std::vector<Army *> selected);

        virtual Army* higherFitnessArmy();
        virtual std::vector<Army*>& getSelectedArmies();

        InitManager *init;
        CrossoverManager *crossover;
        MutationManager *mutation;
        Objective *objective;
};

#endif
