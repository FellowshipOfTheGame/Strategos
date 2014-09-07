/**
* Action.h - Atualizado em: 30/10/2011 - 16:20
*
*   Este arquivo define a class Action. Esta classe tem como
* principal objetivo ... DEFINIR
*
* Fellowship of the Game (FoG) - Universidade de Sao Paulo
* Guilherme Simao Gibertoni / Ivan Caramello / Marcius Leandro Junior
*/

#ifndef _ACTION_H_
#define _ACTION_H_

#include "Image.h"
#include "Ship.h"
#include "Coordinates.h"
#include "Dictionary.h"

void printNActions();

class Action
{
    public:
        Action();
        virtual ~Action();

        bool complete;

        virtual Action* act() = 0;
        virtual bool completed()=0;
        virtual void render();
};

class ExplosionAction : public Action
{
    public:
        ExplosionAction(Coordinates pos, const shipEffects *gfx_sfx);

        Action* act() override;
        bool completed() override;
        void render() override;

    private:
        int timer;
        Coordinates position;
        const shipEffects *gfxsfx;
};

class DamageAction : public Action
{
    public:
        DamageAction(Coordinates pos);

        Action* act() override;
        bool completed() override;
        void render() override;

    private:
        int timer;
        Coordinates position;
};

class MoveAction : public Action
{
    public:
        MoveAction(Ship *Source, const Coordinates& Coord, bool GetNear=false);
        Action* act() override;
        bool completed() override;
        void render() override;

        Ship *source;
        Coordinates coord;

    private:
        bool getNearTo; // Se ativo, faz com que a acao ocorra ate que o alvo fique dentro
                        // da distancia da nave source.
};

class AttackAction : public Action
{
    private:
        double direction;
        int frame;
        Image *shootEffect;

    public:
        AttackAction(Ship *Source, Ship *Target);
        ~AttackAction(){}

        Action* act() override;
        bool completed() override;
        void render() override;

        Ship *source, *target;
        Coordinates coord;
};

class KamikazeAction : public Action
{
public:
    KamikazeAction(Ship *Source, Ship *Target, Image *imgShoot);
	~KamikazeAction(){}

	Action* act() override;
	bool completed() override;
	void render() override;

	Ship *source, *target;
	Coordinates coord;

	float distance;
	Image *explosionEffect;
};
#endif
