#include "CombatLog.h"
CombatRoundItem::CombatRoundItem(int s, double d)
{
	step=s;
	roundDamage = d;
}

CombatRoundItem::CombatRoundItem(int s)
{
	step=s;
	roundDamage = 0;
}

CombatRoundItem::CombatRoundItem(CombatRoundItem* origin)
{
	step=origin->getStep();
	roundDamage = origin->getRoundDamage();
}

int CombatRoundItem::getRoundDamage() const
{
	return roundDamage;
}

void CombatRoundItem::addRoundDamage(int roundDamage)
{
	this->roundDamage += roundDamage;
}

int CombatRoundItem::getStep()
{
	return step;
}

CombatRoundItem* CombatRoundItem::clone()
{
	return new CombatRoundItem(this);
}

bool CombatRoundItem::operator < (CombatRoundItem* origin) const
{
   return (step < origin->getStep());
}

bool CombatRoundItem::operator == ( CombatRoundItem* origin) const
{
   return (step == origin->getStep());
}

bool CombatRoundItem::compareMinor(CombatRoundItem *i, CombatRoundItem *j)
{
	return (i->getStep() < j->getStep());
}

bool CombatRoundItem::compareEqual(CombatRoundItem *i, CombatRoundItem *j)
{
	return (i->getStep() == j->getStep());
}

bool CombatRoundItem::compareMaxDamage(CombatRoundItem *i, CombatRoundItem *j)
{
	return (i->getRoundDamage() < j->getRoundDamage());
}


/*****************************************************************************/

CombatRound::CombatRound()
{
}

CombatRound::CombatRound(std::vector<CombatRoundItem*>& origin)
{
	std::vector<CombatRoundItem*>::iterator iter = origin.begin();

	while (iter != origin.end())
	{
	    CombatRoundItem *temp = ((*iter)->clone());
		log.push_back( temp );
		iter++;
	}
	std::sort(log.begin(), log.end(),CombatRoundItem::compareMinor);

}

CombatRound::~CombatRound()
{
	std::vector<CombatRoundItem*>::const_iterator iter = log.begin();

	while (iter != log.end())
	{
		delete (*iter);
		iter++;
	}
	log.clear();
}

const std::vector<CombatRoundItem*>& CombatRound::getLog() const
{
	return log;
}

void CombatRound::addLog(CombatRoundItem *round)
{
	if (log.size()<1)
	{
		log.push_back(round->clone());
		return;
	}

	std::sort(log.begin(), log.end(),CombatRoundItem::compareMinor);
	std::vector<CombatRoundItem*>::const_iterator iter = log.begin();

	while (iter != log.end())
	{
		if (CombatRoundItem::compareEqual(round,(*iter)))
		{
			(*iter)->addRoundDamage(round->getRoundDamage());
			return;
		}
		iter++;
	}
	log.push_back(round->clone());


}

CombatRound* CombatRound::ConcatCombatRound(CombatRound* cb)
{
	CombatRound* _new = new CombatRound(log);

	std::vector<CombatRoundItem*>::const_iterator iter = cb->getLog().begin();

	while (iter != cb->getLog().end())
	{
		_new->addLog((*iter));
		iter++;
	}

	return _new;
}

void CombatRound::print()
{
	printf ("imprimindo combat round\n \t");
	std::vector<CombatRoundItem*>::iterator iter = log.begin();
	while (iter != log.end())
	{
		printf ("[%d, %d] ",(*iter)->getStep(),(*iter)->getRoundDamage());
		iter++;
	}
	printf ("\n");
}

/*****************************************************************************/

CombatLogItem::CombatLogItem(int squad, int ship)
{
	this->squadNum=squad;
	this->shipNum = ship;
}

CombatLogItem::~CombatLogItem()
{
}
/*****************************************************************************/

CombatLog::CombatLog()
{
	this->damageDealt=0;
	this->damageTaken=0;
	this->combatRegister = new CombatRound();
}

CombatLog::~CombatLog()
{
	std::vector<CombatLogItem*>::iterator iter = combatline.begin();
	while (iter != combatline.end())
	{
		delete (*iter);
		iter++;
	}
	this->combatline.clear();
}

void CombatLog::addLog(int squad, int ship)
{
	this->combatline.push_back(new CombatLogItem(squad,ship));
}

std::vector<CombatLogItem*> CombatLog::getLog()
{
	return this->combatline;
}


void CombatLog::setRegister(CombatRound *cb)
{
	CombatRound *_new;
	_new = this->combatRegister->ConcatCombatRound(cb);
	delete this->combatRegister;
	this->combatRegister = _new;

	const std::vector<CombatRoundItem*>& log = this->combatRegister->getLog();
	std::vector<CombatRoundItem*>::const_iterator iter = log.begin();

	while (iter != log.end())
	{
		this->damageTaken += (*iter)->getRoundDamage();
		iter++;
	}
}
CombatRound* CombatLog::getRegister()
{
	return this->combatRegister;
}
