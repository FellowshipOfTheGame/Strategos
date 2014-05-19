#ifndef _CROSSOVER_MANAGER_
#define _CROSSOVER_MANAGER_

#include "Army.h"
#include <vector>
class CrossoverManager
{
	public:
		void crossOver(const Army *parent1, const Army *parent2, Army *&child1, Army *&child2);

		CrossoverManager();
		virtual ~CrossoverManager();

};

#endif