/*
 * CombatLog.h
 *
 *  Created on: 31/08/2013
 *      Author: Eldhelion
 */
#ifndef COMBATLOG_H_
#define COMBATLOG_H_

#include "Army.h"
#include "Unit.h"
#include "Ship.h"
#include <time.h>

using namespace std;

class CombatLogItem
{
	private:
		time_t time;
		int squadNum, shipNum;
	public:
		CombatLogItem(int squad, int ship);
		~CombatLogItem();

		int getShipNum() const
		{
			return shipNum;
		}

		int getSquadNum() const
		{
			return squadNum;
		}

		time_t getTime() const
		{
			return time;
		}
};

class CombatLog
{
	private:
		std::vector<CombatLogItem*> combatline;
		int damageDealt, damageTaken;
	public:
		CombatLog();
		~CombatLog();
		void addLog(int squad, int ship);
		std::vector<CombatLogItem*> getLog();

		int getDamageDealt() const
		{
			return damageDealt;
		}

		void setDamageDealt(int damageDealt)
		{
			this->damageDealt += damageDealt;
		}

		int getDamageTaken() const
		{
			return damageTaken;
		}

		void setDamageTaken(int damageTaken)
		{
			this->damageTaken += damageTaken;
		}
		int getSize()
		{
			return combatline.size();
		}
};


#endif /* COMBATLOG_H_ */
