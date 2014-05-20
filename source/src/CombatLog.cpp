#include "CombatLog.h"

CombatLog::CombatLog(int nUnits)
    : unitLog(nUnits)
{
}

CombatRound* CombatLog::getLogForUnit(int unitID)
{
    return &unitLog[unitID];
}

void CombatLog::nextStep()
{
    for (unsigned int i = 0; i < unitLog.size(); ++i)
        unitLog[i].nextStep();
}

void CombatLog::calculateGeneralLog()
{
    for (unsigned int i = 0; i < unitLog.size(); ++i)
    {
        const LogMap& unit_log = unitLog[i].getLog();
        LogMap::const_iterator it = unit_log.begin();

        while (it != unit_log.end())
        {
            generalLog.addLog( it->first, it->second );
            ++it;
        }
    }
}

const CombatRound* CombatLog::getGeneralLog() const
{
    return &generalLog;
}

/*****************************************************************************/

CombatRound::CombatRound()
    : current_step(0)
{
}

CombatRound::~CombatRound()
{
}

const LogMap& CombatRound::getLog() const
{
	return log;
}

void CombatRound::addLog(const RoundData& data)
{
    addLog(current_step, data);
}

void CombatRound::addLog(int time, const RoundData& data)
{
    if ( log.find(time) != log.end() ){
        log[time] += data;
    }else{
        log[time] = data;
    }
}

void CombatRound::setLog(int time, const RoundData& data)
{
    log[time] = data;
}

void CombatRound::print() const
{
	printf ("imprimindo combat round\n \t");
	LogMap::const_iterator iter = log.begin();
	printf("LoggedTimes: %d\n", log.size());
	while (iter != log.end())
	{
		printf ("[%d, %.1lf, %.1lf, %d, %d] ", iter->first, iter->second.damageReceived,
                                    iter->second.damageDealt, iter->second.deaths, iter->second.kills);
		++iter;
	}
	printf ("\n");
}

void CombatRound::nextStep()
{
    ++current_step;
}

int CombatRound::getLastLoggedTime() const
{
    if (log.empty())
        return 0;

    return log.rbegin()->first;
}

int CombatRound::getFirstLoggedTime() const
{
    if (log.empty())
        return 0;

    return log.begin()->first;
}

RoundData CombatRound::getMaximumData() const
{
    RoundData maxData;

    for (LogMap::const_iterator it = log.begin(); it != log.end(); ++it)
    {
        const RoundData& chkdata = it->second;

        // Guardar maximos
        if (chkdata.damageDealt > maxData.damageDealt)
            maxData.damageDealt = chkdata.damageDealt;
        if (chkdata.damageReceived > maxData.damageReceived)
            maxData.damageReceived = chkdata.damageReceived;
        if (chkdata.deaths > maxData.deaths)
            maxData.deaths = chkdata.deaths;
        if (chkdata.kills > maxData.kills)
            maxData.kills = chkdata.kills;
    }

    return maxData;
}

/*****************************************************************************/
