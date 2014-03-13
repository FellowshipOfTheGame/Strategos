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

class Action
{
    public:
        Action(){complete = 0;}
        virtual ~Action(){}

        bool complete;

        virtual Action* act() = 0;
        virtual bool completed(){return 1;}
        virtual void render( float camOX, float camOY){}
};

class ExplosionAction : public Action
{
    public:
        ExplosionAction(Coordinates pos, Image *explosionImg);

        Action* act();
        bool completed();
        void render(float camOX, float camOY);

    private:
        Image* img;
        int timer;
        Coordinates position;
};

class DamageAction : public Action
{
    public:
        DamageAction(Coordinates pos);

        Action* act();
        bool completed();
        void render(float camOX, float camOY);

    private:
        int timer;
        Coordinates position;
};

class MoveAction : public Action
{
public:
	MoveAction(Ship *Source, Coordinates Coord);
	MoveAction(Ship *Source, Ship* target);
	Action* act();
	bool completed();
	void render(float camOX, float camOY);

    Ship *source, *target;
	Coordinates coord;
};

class AttackAction : public Action
{
    private:
        double direction;
        int frame;
        const DictKey *sourceInfo;
        const DictKey *targetInfo;

    public:
        AttackAction(Ship *Source, Ship *Target, const DictKey *sourceInfo, const DictKey *targetInfo);
        ~AttackAction(){}

        Action* act();
        bool completed();
        void render( float camOX, float camOY );

        Ship *source, *target;
        Coordinates coord;

        Image *shootEffect;
};

class KamikazeAction : public Action
{
public:
		KamikazeAction(Ship *Source, Ship *Target, Image *imgShoot);
	~KamikazeAction(){}

	Action* act();
	bool completed();
	void render( float camOX, float camOY );

	Ship *source, *target;
	Coordinates coord;

	float distance;
	Image *explosionEffect;
};
#endif
