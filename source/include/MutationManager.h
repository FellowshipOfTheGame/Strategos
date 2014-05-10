#ifndef _MUTATION_MANAGER_
#define _MUTATION_MANAGER_

#include <Army.h>
#include <Tactic.h>


class MutationManager
{
	private:
		void mutate(Army *ind, int degree);
		void mutateUnitType(Army* ind, int unitID, int newType);
		void mutateTactic(Tactic *tactic, int degree);
	public:
		MutationManager();
		virtual ~MutationManager();

		void mutate(Army *ind);
		
};

#endif