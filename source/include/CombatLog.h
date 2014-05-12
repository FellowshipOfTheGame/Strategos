#ifndef COMBATLOG_H_
#define COMBATLOG_H_

#include <vector>
#include <map>

struct RoundData
{
    RoundData()
        : damageReceived(0), damageDealt(0), deaths(0), kills(0)
            {}

    RoundData(double dR, double dD, int d, int k)
        : damageReceived(dR), damageDealt(dD), deaths(d), kills(k)
            {}

    const RoundData& operator +=(const RoundData& x)
    {
        damageReceived += x.damageReceived;
        damageDealt += x.damageDealt;

        deaths += x.deaths;
        kills += x.kills;

        return *this;
    }

    const RoundData& operator =(const RoundData& x)
    {
        damageReceived = x.damageReceived;
        damageDealt = x.damageDealt;

        deaths = x.deaths;
        kills = x.kills;

        return *this;
    }

    double damageReceived;
    double damageDealt;

    int deaths;
    int kills;
};

typedef std::map<int, RoundData> LogMap;

class CombatRound
{
	private:
		LogMap log;
		int current_step;

	public:
		CombatRound();
		~CombatRound();

		const LogMap& getLog() const;

		void addLog(const RoundData& data);
		void addLog(int time, const RoundData& data);

		void print() const;

		void nextStep();

		int getLastLoggedTime() const;
		int getFirstLoggedTime() const;

		/// Calcula maximo na hora, O(n)!
        RoundData getMaximumData() const;
};

// Log de uma army
class CombatLog
{
    private:
        // Guarda o log geral de todas as unidades
        CombatRound generalLog;

        // Guarda o log especifico para cada Unit
        std::vector<CombatRound> unitLog;

    public:
        CombatLog(int nUnits);

        CombatRound* getLogForUnit(int unitID);
        const CombatRound* getGeneralLog() const;
        void nextStep();

        void calculateGeneralLog();
};

#endif /* COMBATLOG_H_ */
