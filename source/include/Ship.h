#ifndef _SHIP_H_
#define _SHIP_H_

#define SPACIAL_UNIT 32.0

#include "CombatLog.h"
#include "Coordinates.h"
#include "RandomEngine.h"

struct shipBaseStats
{
    shipBaseStats()
    {
    }

    double maxHP;
	double damage;
	int maxAtkCD;   // Cooldown de ataque
	double range;
	double shield;
	double dodge;
	double speed;
};

enum SHIP_MOVE{
    move_not_moving,
    move_action,
    move_flying
};

struct shipCurrentStats
{
    shipCurrentStats(const shipBaseStats &base)
        : isKamikasing(false), isMoving(move_not_moving)
    {
        currentHP = base.maxHP;
        currentShield = base.shield;
        curKamikazeCD = 0;
        currentAtkCD = 0;
        currentSpeed = base.speed;
    }

    double currentHP;
	int currentAtkCD;   // Cooldown de ataque
	int curKamikazeCD;
	double currentShield;
	double currentSpeed;

    // Actions
	bool isKamikasing;

	// 0 = not, 1 = move from action, 2 flying arround
	SHIP_MOVE isMoving;
};

class Ship
{
    private:
        int owner;
        int status;
        float currentDirection;

        // Atributos iniciados no construtor
        const shipBaseStats &baseStats;
        shipCurrentStats stats;

        // coord = posicao atual
        Coordinates coord, unitPos, targetPos;

        CombatRound* myLog;

    public:
        Ship(const shipBaseStats &initialStats, const Coordinates& Coord, CombatRound* log=nullptr);
        ~Ship();

        int update(RandomEngine& randE);
        bool isAlive() const;

        SHIP_MOVE getMoving() {return stats.isMoving;}

        /// \return: Retorna se o dano foi letal
        bool takeDamage(double damage);

        ///
        bool dealDamage(double damage, bool wasLetal);

        // Seleciona a posicao "alvo" para a nave ir em direcao
        void moveTo(const Coordinates& c);

        float getDirection() const;

        double getHP() const;
        void kill();

        const shipBaseStats &getBaseStats() const;
        shipCurrentStats &getStats();

        int getX() const;
        int getY() const;
        const Coordinates& getPosition() const;
        const Coordinates& getTargetPos() const;
};

#endif
