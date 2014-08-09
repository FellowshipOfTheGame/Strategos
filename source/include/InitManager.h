#ifndef _INIT_MANAGER_
#define _INIT_MANAGER_

#include "Army.h"

class InitManager
{
	public:
		InitManager(int _armyType);
		virtual ~InitManager();

		Army *generateRandomArmy();

		static Tactic* generateRandomTactic( const Army* forArmy, int forUnitID );
		static Trigger* generateRandomTrigger();

	private:
		int armyType;
};

#endif
