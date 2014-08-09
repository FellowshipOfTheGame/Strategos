//
//  GeneticAlgorithm.h
//  Strategos
//
//

#ifndef _GENETIC_ALGORITHM_
#define _GENETIC_ALGORITHM_

#include "Algorithm.h"
#include "Army.h"
#include "InitManager.h"
#include "CrossoverManager.h"
#include "MutationManager.h"
#include "Objective.h"

struct PairAF
{
    Army *ind;
    double fitness;

    PairAF(Army *army, double fit)
        : ind(army), fitness(fit)
    {}
};

class GeneticAlgorithm : public Algorithm
{
    public:
        GeneticAlgorithm(int _armyType);
        ~GeneticAlgorithm();

        void run() override;

        void selection();
        void randomArmies(int size);
        void normalizeFitness(std::vector<PairAF> *pairs);

        static bool highToLow(PairAF p1, PairAF p2)
        {
            return (p1.fitness > p2.fitness);
        }

    protected:
        void addInitialArmy(Army *army) override;

    private:
        std::vector<Army*> individuos;

        int allowedThreads;

        /**
         * \param selected Retorna os individuos selecionados para a PROXIMA geracao
         *          Ordenados em ordem decrescente de fitness
         * \param n Numero de individuos para selecionar
         */
        void selectFromPop(unsigned int n, std::vector<Army*>& selected, std::vector<Army*>& rejected);

        /// Efetua o crossover ateh popular os INDIVIDUOS_GERACAO indivudos
        void crossOver(std::vector<Army*>& selected);

        /// Efetua mutacao nos individuos
        void mutate(std::vector<Army*>& selected);

        //
        void threadSimulate( unsigned int from, unsigned int n );

        void repair(std::vector<Army *> selected);

        Army* higherFitnessArmy() override;
        std::vector<Army*>& getSelectedArmies() override;

        InitManager *init;
        CrossoverManager *crossover;
        MutationManager *mutation;
        Objective *objective;
};

#endif
