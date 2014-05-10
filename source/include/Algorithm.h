#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "Army.h"

// Classe base para Algoritmo Genetico

class Algorithm
{
    private:
    	void createNeededDirectory();
        std::string directory;
    protected:
    	virtual void addInitialArmy(Army *army) = 0;
        void saveArmy(Army *army, int armyNumber);
        int armyType;
    public:
        Algorithm(int _armyType);
        virtual ~Algorithm();

        //talvez nao use isso
        virtual Army* higherFitnessArmy() = 0;
        virtual std::vector<Army*>& getSelectedArmies() = 0;

        virtual void initialize();
        virtual void run() = 0;
};

#endif // ALGORITHM_H
