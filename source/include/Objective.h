#ifndef _OBJECTIVE_
#define _OBJECTIVE_

#include <Army.h>

class Objective
{
	private:
		
	public:
		//metodos de reparo
		void rectifyUnit(Army *ind);
        void GoldCap(Army *army);

        //calcula o fitness de uma batalha
        double calculateFitness(const Army *ind, int simSteps);
        //simula uma batalha
        int fight(Army *a, Army *b, int *steps);

        //calcula o fitness de duas estrategias previamente selecionadas
        void evaluate(Army *a, Army *b, double *fitA, double *fitB);


};

#endif