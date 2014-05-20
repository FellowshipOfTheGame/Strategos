/**
* Action.cpp - Atualizado em: 30/10/2011 - 16:20
*
* Este arquivo contem o codigo-fonte da class Action.
* Contem ... DEFINIR
*
* Fellowship of the Game (FoG) - Universidade de Sao Paulo
* Guilherme Simao Gibertoni / Ivan Caramello / Marcius Leandro Junior / Mateus Malvessi
*/

#include "Global.h"
#include "Game.h"
#include "Action.h"
#include "World.h"
#include "Unit.h"

#include "SDL2_gfx/SDL2_gfxPrimitives.h"

static int _N_ACTIONS_ = 0;
static int _N_ACTIONS_EXPLOSION_ = 0;
static int _N_ACTIONS_DMG_ = 0;
static int _N_ACTIONS_MOVE_ = 0;

void printNActions(){
    printf("TOTAL Actions: %d\n", _N_ACTIONS_);
    printf("TOTAL Explosions: %d\n", _N_ACTIONS_EXPLOSION_);
    printf("TOTAL Damage: %d\n", _N_ACTIONS_DMG_);
    printf("TOTAL Move: %d\n", _N_ACTIONS_MOVE_);
    _N_ACTIONS_ = 0;
    _N_ACTIONS_EXPLOSION_ = 0;
    _N_ACTIONS_DMG_ = 0;
    _N_ACTIONS_MOVE_ = 0;
}

Action::Action()
    : complete(0)
{
    ++_N_ACTIONS_;
}

Action::~Action(){

}

void Action::render(){

}

///

ExplosionAction::ExplosionAction(Coordinates pos, Image *explosionImg)
    : img(explosionImg), timer(0), position(pos)
{
    ++_N_ACTIONS_EXPLOSION_;
}

Action* ExplosionAction::act()
{
    ++timer;

    if (timer >= img->getNumberFrames())
        complete = true;

    return 0;
}

bool ExplosionAction::completed()
{
    return complete;
}

void ExplosionAction::render()
{
    // Explosion
    img->DrawImage(Game::getGlobalGame()->getRenderer(), position.x, position.y, timer);
}


///


DamageAction::DamageAction(Coordinates pos)
    : timer(0), position(pos)
{
    ++_N_ACTIONS_DMG_;
}

Action* DamageAction::act()
{
    ++timer;

    if (timer >= FRAMES_PER_SECOND/4)
        complete = true;

    return 0;
}

bool DamageAction::completed()
{
    return complete;
}

void DamageAction::render()
{
    // Damage
    int rad = 26;
    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x + (rand()%rad-rand()%rad), position.y + (rand()%rad-rand()%rad), (rand()%3+1)/(timer/5+1), 168+rand()%88, 50+rand()%128, 50+rand()%128, 190+rand()%55 );
    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x + (rand()%rad-rand()%rad), position.y + (rand()%rad-rand()%rad), (rand()%3+1)/(timer/5+1), 168+rand()%88, 50+rand()%128, 50+rand()%128, 190+rand()%55 );
    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x + (rand()%rad-rand()%rad), position.y + (rand()%rad-rand()%rad), (rand()%3+1)/(timer/5+1), 168+rand()%88, 50+rand()%128, 50+rand()%128, 190+rand()%55 );
    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x + (rand()%rad-rand()%rad), position.y + (rand()%rad-rand()%rad), (rand()%3+1)/(timer/5+1), 168+rand()%88, 50+rand()%128, 50+rand()%128, 190+rand()%55 );
    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x + (rand()%rad-rand()%rad), position.y + (rand()%rad-rand()%rad), (rand()%3+1)/(timer/5+1), 168+rand()%88, 50+rand()%128, 50+rand()%128, 190+rand()%55 );
}

///

MoveAction::MoveAction(Ship *Source, const Coordinates& Coord, bool GetNear)
    : source(Source), coord(Coord), getNearTo(GetNear)
{
    ++_N_ACTIONS_MOVE_;

    if (getNearTo)
    {
        double dist = Coord.distance(source->getPosition());

        if (dist >= source->getBaseStats().range)
        {
            double direction = atan2(source->getY() - Coord.y, source->getX() - Coord.x);
            coord.x = source->getX() - (dist-source->getBaseStats().range*0.9) * cos(direction);
            coord.y = source->getY() - (dist-source->getBaseStats().range*0.9) * sin(direction);
        }
    }else{

            // !!!! Ja foi iniciado com Coord !!!!
            // coord = Coord;

    }
}

Action* MoveAction::act()
{
    source->moveTo( coord );
    complete = 1;

    return nullptr;
}

bool MoveAction::completed()
{
    return complete;
}

void MoveAction::render()
{
//    lineRGBA(Game::getGlobalGame()->getScreenSurface(), source->getX() - cOX, source->getY() - cOY,
//                coord.x - cOX, coord.y - cOY, 255, 0, 0, 32);
}

AttackAction::AttackAction(Ship *Source, Ship *Target, const DictKey *srcInfo, const DictKey *trgetInfo)
    : frame(0), sourceInfo(srcInfo), targetInfo(trgetInfo),
        source(Source), target(Target),
        coord(Source->getPosition()), shootEffect(srcInfo->shootGFX)
{
}

Action* AttackAction::act()
{
    if (!target->isAlive())
    {
        complete = 1;
        //printf("Tiro \"perdido\" no espaco!\n");
        return nullptr;
    }

    float dist = coord.distance(target->getPosition());
    if ( dist < 5.0f )
    {
        complete = 1;

        bool letal = target->takeDamage(source->getBaseStats().damage);
        source->dealDamage( source->getBaseStats().damage, letal );

        if ( letal )
            return new ExplosionAction(coord, targetInfo->explosionGFX);
        else
            return new DamageAction(coord);
    }
    else
    {
        direction = atan2(coord.y - target->getPosition().y, coord.x - target->getPosition().x);

        float speed = std::min( dist, 16.0f );

        coord.x -= speed*cos(direction);
        coord.y -= speed*sin(direction);
    }

    ++frame;
    if (frame > shootEffect->getNumberFrames())
        frame = 0;

    return nullptr;
}

bool AttackAction::completed()
{
    return complete;
}

void AttackAction::render( )
{
    // LAZER
//    float direction = atan2(coord.y - target->getPosition().y, coord.x - target->getPosition().x);
//    float x = -cos(direction);
//    float y = -sin(direction);
//    float distI = source->getPosition().distance(target->getPosition());
//    float dist = 255 - coord.distance(target->getPosition())/distI * 255;
//    lineRGBA( Game::getGlobalGame()->getRenderer(), coord.x - cOX, coord.y - cOY,
//             target->getX()- cOX, target->getY() - cOY, 255, rand()%64, rand()%64, dist );

//    lineRGBA( Game::getGlobalGame()->getScreenSurface(), source->getX() + dist*x - cOX, source->getY() + dist*y - cOY,
//             target->getX()- cOX, target->getY() - cOY, 255, rand()%64, rand()%64, 64 );

    // BUBBLE
//    circleRGBA( Game::getGlobalGame()->getScreenSurface(), coord.x - cOX, coord.y - cOY, 3, 128+rand()%128, rand()%128, rand()%128, 200+rand()%55 );
//    circleRGBA( Game::getGlobalGame()->getScreenSurface(), coord.x - cOX, coord.y - cOY, 1, 128+rand()%128, rand()%128, rand()%128, 200+rand()%55 );

    // Normal
    shootEffect->DrawImage(Game::getGlobalGame()->getRenderer(), (int)coord.x, (int)coord.y, frame, ( (direction+M_PI) * 180.0) /M_PI);
}

KamikazeAction::KamikazeAction(Ship *Source, Ship *Target, Image *imgShoot)
    :   source(Source), target(Target), coord(Source->getPosition()),
        distance(0), explosionEffect(imgShoot)
{
}

Action* KamikazeAction::act()
{
    if ( !target->isAlive() )
    {
        complete = 1;
        source->kill();

        return 0;
    }

    double dist = source->getPosition().distance(target->getPosition());
    if ( dist < 15.0 )
    {
    	double damage = source->getBaseStats().damage * distance/2.0;
        bool letal = target->takeDamage( damage );
        source->dealDamage( damage, letal );
    	source->kill();

        complete = 1;
    }
    else
    {
        distance += 1;
        source->moveTo(target->getPosition());
    }

    return 0;
}

bool KamikazeAction::completed()
{
    return complete;
}

void KamikazeAction::render( )
{
   // explosionEffect->setFrame(0);
   // explosionEffect->DrawImage(coord.x - camOX, coord.y - camOY, Game::getGlobalGame()->getScreenSurface());
}
