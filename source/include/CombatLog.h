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
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

class CombatRoundItem
{
	private:
		int step;
		double roundDamage;
	public:
		CombatRoundItem(int s, double d);
		CombatRoundItem(int s);
		CombatRoundItem(CombatRoundItem* origin);

		void addRoundDamage(int roundDamage);

		int getRoundDamage() const;
		int getStep();

		CombatRoundItem* clone();
		bool operator <(CombatRoundItem* origin) const;
		bool operator ==(CombatRoundItem* origin) const;
};

class CombatRound
{
	private:
		std::vector<CombatRoundItem*> log;
	public:
		CombatRound();
		CombatRound(std::vector<CombatRoundItem*>& origin);
		~CombatRound();
		const std::vector<CombatRoundItem*>& getLog() const;

		void addLog(CombatRoundItem *round);

		CombatRound* ConcatCombatRound(CombatRound* cb);

};

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
		CombatRound *combatRegister;
		int damageDealt, damageTaken;
	public:
		CombatLog();
		~CombatLog();
		void addLog(int squad, int ship);
		std::vector<CombatLogItem*> getLog();

		int getTotalDamageDealt() const
		{
			return damageDealt;
		}

		int getTotalDamageTaken() const
		{
			return damageTaken;
		}

		int getLogSize()
		{
			return combatline.size();
		}

		void setRegister(CombatRound *cb);
		CombatRound* getRegister();
};

#endif /* COMBATLOG_H_ */
