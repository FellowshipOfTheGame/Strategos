#ifndef _CROSSOVER_MANAGER_
#define _CROSSOVER_MANAGER_

#include "Army.h"
#include <vector>
class CrossoverManager
{
	public:
		/// Efetua o crossover entre dois individuos colocando 2 filhos no vetor ind
		void crossOver(const Army *parent1, const Army *parent2, std::vector<Army*>& ind);

		CrossoverManager();
		virtual ~CrossoverManager();

};

#endif