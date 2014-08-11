#ifndef _MUTATION_MANAGER_
#define _MUTATION_MANAGER_

#include "Army.h"
#include "Tactic.h"

enum ARMY_MUTATION_KIND{
    AMK_UNIT_TYPE = 0,
    AMK_UNIT_TACTIC,
    AMK_UNIT_POSITION,
    AMK_UNIT_INSERT_NEW,

    AMK_UNIT_TOTAL
};

enum TACTIC_MUTATION_KIND{
    TMK_TRIGGER_TYPE = 0,
    TMK_TRIGGER_VALUE, // Valor do trigger. Ex: 70 [%]
    TMK_TRIGGER_REL_OP, // Operador de relacao = < >
    TMK_TRIGGER_LOGIC_OP, // Operador logico AND / OR

    TMK_MUTATION_TOTAL // Total de tipos de mutacao
};

class MutationManager
{
	public:
		MutationManager();
		virtual ~MutationManager();

		void mutate(Army *ind);

    private:
		void mutate(Army *ind, ARMY_MUTATION_KIND degree);
		void mutateUnitType(Army* ind, int unitID, int newType);
		void mutateTactic(Tactic *tactic, TACTIC_MUTATION_KIND degree);

};

#endif
