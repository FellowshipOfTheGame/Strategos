//
//  GeneticAlgorithm.h
//  Strategos
//
//

#ifndef _GENETIC_ALGORITHM_
#define _GENETIC_ALGORITHM_

#include <stdio.h>

#include "Algorithm.h"
#include "Army.h"
#include "Tactic.h"

#define INDIVIDUOS_GERACAO  30
#define SELECT_FROM_POP 20
#define GOLD_AMOUNT 400

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
        GeneticAlgorithm(int _armyType) : Algorithm(),
            armyType(_armyType)
        {
            directory = "GA/"+std::to_string(armyType)+"/";
        }
        ~GeneticAlgorithm();

        void initialize() override;
        void run() override;
        double evaluateFitness(const Army *ind);
        void selection();
        void randomArmies(int size);
        void normalizeFitness(std::vector<PairAF> *pairs);
        void rectifyUnitID(Army *ind);
        void GoldCap(Army *army);

        static bool highToLow(PairAF p1, PairAF p2)
        {
            return (p1.fitness > p2.fitness);
        }

        Army *generateRandomArmy();

        //talvez nao use isso
        Army* higherFitnessArmy();
        std::vector<Army*>& getSelectedArmies();

        // Mutation
        void mutation(Army *ind, int degree);
        void mutateUnitType(Army* ind, int unitID, int newType);
        void mutateTactic(Tactic *tactic, int degree);

    private:
        int armyType;
        std::string directory;
        std::vector<Army*> individuos;

        /**
         * \param selected Retorna os individuos selecionados para a PROXIMA geracao
         *          Ordenados em ordem decrescente de fitness
         * \param n Numero de individuos para selecionar
         */
        void selectFromPop(int n, std::vector<Army*>& selected, std::vector<Army*>& rejected);

        /// Efetua o crossover ateh popular os INDIVIDUOS_GERACAO indivudos
        void crossOver(std::vector<Army*>& selected);

        /// Efetua o crossover entre dois individuos colocando 2 filhos no vetor ind
        void crossOver(const Army *parent1, const Army *parent2, std::vector<Army*>& ind);

        /// Efetua mutacao nos individuos
        void mutate(std::vector<Army*>& selected);
};

#endif
