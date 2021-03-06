#ifndef _SHIP_H_
#define _SHIP_H_

#define SPACIAL_UNIT 32.0
#define SPACIAL_UNIT_2 (SPACIAL_UNIT*SPACIAL_UNIT)

#include "CombatLog.h"
#include "Coordinates.h"
#include "RandomEngine.h"
#include "SoundManager.h"
#include "Image.h"

struct shipEffects
{
    shipEffects() :
        shipIdle(nullptr), shootGFX(nullptr), explosionGFX(nullptr)
        {
            explosionsSFX[0] = explosionsSFX[1] = explosionsSFX[2] = explosionsSFX[3] = nullptr;
            shootSFX = nullptr;
        }

    Image *shipIdle;

    Image *shootGFX;
    Image *explosionGFX;

    Mix_Chunk* explosionsSFX[4]; // 4 Variacoes de som
    Mix_Chunk* shootSFX;
};

struct shipBaseStats
{
    shipBaseStats()
    {
    }

    float maxHP;
	double damage;
	int maxAtkCD;   // Cooldown de ataque
	double range;
	double shield;
	double dodge;
	double speed;

	double range2; //!> range ao quadrado.
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

    float currentHP;
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
        //int owner;
        int status;
        float currentDirection;

        // Atributos iniciados no construtor
        const shipBaseStats &baseStats;
        shipCurrentStats stats;
        const shipEffects* shipeffects;

        // coord = posicao atual
        Coordinates coord, unitPos, targetPos;

        CombatRound* myLog;

        bool worldType;

    public:
        Ship(const shipBaseStats &initialStats, const Coordinates& Coord, bool world_type, CombatRound* log=nullptr, const shipEffects* ship_effects=nullptr);
        ~Ship();

        int update(RandomEngine& randE);
        void draw();
        bool isAlive() const;

        SHIP_MOVE getMoving() {return stats.isMoving;}

        /// \return: Retorna se o dano foi letal
        bool takeDamage(double damage);

        ///
        void dealDamage(double damage, bool wasLetal);

        // Seleciona a posicao "alvo" para a nave ir em direcao
        void moveTo(const Coordinates& c);

        float getDirection() const;

        bool getWorldType() const;
        float getHP() const;
        void kill();

        const shipBaseStats &getBaseStats() const;
        shipCurrentStats &getStats();
        const shipEffects* getEffects() const;

        int getX() const;
        int getY() const;
        const Coordinates& getPosition() const;
        const Coordinates& getTargetPos() const;
};

#endif
