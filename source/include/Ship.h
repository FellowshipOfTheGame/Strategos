// 2011-05-04; Lin

#ifndef _SHIP_H_
#define _SHIP_H_

#include "Coordinates.h"

using namespace std;

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

struct shipCurrentStats
{
    shipCurrentStats(const shipBaseStats &base)
    {
        currentHP = base.maxHP;
        currentShield = base.shield;
        curKamikazeCD = 0;
        currentAtkCD = 0;
    }

    double currentHP;
	int currentAtkCD;   // Cooldown de ataque
	int curKamikazeCD;
	double currentShield;
};

class Ship
{
    private:
        static unsigned long gid;
        unsigned long id;
        int owner;

        int deathTime, status;

        float direction;

        // Atributos iniciados no construtor
        const shipBaseStats &baseStats;
        shipCurrentStats stats;
        // coord = posicao atual
        Coordinates coord, basePos, targetPos;
        bool moving;

    public:
        Ship(const shipBaseStats &initialStats, Coordinates Coord, int shipID);
        ~Ship();

        int update();
        bool isAlive();

        /// \return: Retorna se o dano foi letal
        bool takeDamage(double damage);

        void move(Coordinates delta);
        void move(double dx, double dy);
        void moveTo(Coordinates c);

        float getDirection();

        double getHP();
        void kill();

        const shipBaseStats &getBaseStats();
        shipCurrentStats &getStats();

        unsigned long getID();
        int getX();
        int getY();
        const Coordinates& getPosition();
        const Coordinates& getTargetPos();
        int logUpdate();
};

#endif
