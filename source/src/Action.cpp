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
#include "Action.h"
#include "World.h"
#include "Unit.h"
#include "Ship.h"

#include "SDL2_gfxPrimitives.h"

ExplosionAction::ExplosionAction(Coordinates pos, Image *explosionImg)
    : img(explosionImg), timer(0), position(pos)
{
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
    img->DrawImage(position.x, position.y, timer, Game::getGlobalGame()->getRenderer());
//    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x - camOX, position.y - camOY, 28/(timer/2+1), 128+rand()%128, rand()%128, rand()%128, 150+rand()%55 );
//    filledCircleRGBA( Game::getGlobalGame()->getRenderer(), position.x - camOX, position.y - camOY, (14+rand()%16)/(timer/2+1), 128+rand()%128, rand()%128, rand()%128, 150+rand()%55 );
}


///


DamageAction::DamageAction(Coordinates pos)
    : timer(0), position(pos)
{
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

MoveAction::MoveAction(Ship *Source, Coordinates Coord)
    : source(Source), target(0), coord(Coord)
{

}

MoveAction::MoveAction(Ship *Source, Ship* Target)
    : source(Source), target(Target)
{
    Coordinates tc = target->getPosition();
    float dist = tc.distance(source->getPosition());

    if (dist > source->getBaseStats().range)
    {
        float direction = atan2(source->getY() - target->getY(), source->getX() - target->getX());
        dist -= (source->getBaseStats().range-30);
        coord.x = source->getX() - dist * cos(direction);
        coord.y = source->getY() - dist * sin(direction);
    }
    else
    {
        coord = source->getPosition();
    }

//    printf("Move To: %lf, %lf\n", coord.x, coord.y);
}

Action* MoveAction::act()
{
    // Terminar a acao quando chegar proximo do destino
    if (coord.distance(source->getX(), source->getY()) < 5.0)
    {
        complete = 1;
    }
    else
    {
        source->moveTo(coord);
        complete = 1;
    }

    return 0;
}

bool MoveAction::completed()
{
    return complete;

    float dist = sqrt( pow( source->getX()-coord.x, 2)
                     + pow( source->getY()-coord.y, 2) );
    if ( dist < 20 )
        return 1;

    return 0;
}

void MoveAction::render()
{
//    lineRGBA(Game::getGlobalGame()->getScreenSurface(), source->getX() - cOX, source->getY() - cOY,
//                coord.x - cOX, coord.y - cOY, 255, 0, 0, 32);

    if (target)
    lineRGBA(Game::getGlobalGame()->getRenderer(), source->getX(), source->getY() ,
                target->getX(), target->getY(), 255, 0, 0, 32);
}

AttackAction::AttackAction(Ship *Source, Ship *Target, const DictKey *srcInfo, const DictKey *trgetInfo)
    : sourceInfo(srcInfo), targetInfo(trgetInfo), source(Source), target(Target), coord(Source->getPosition()), shootEffect(sourceInfo->shootGFX)
{
    frame = -1;
}

Action* AttackAction::act()
{
    if (!target->isAlive())
    {
        complete = 1;
        //printf("Tiro \"perdido\" no espaco!\n");
        return 0;
    }

    float dist = coord.distance(target->getPosition());
    if ( dist < 5.0f )
    {
        complete = 1;

        if ( target->takeDamage(source->getBaseStats().damage) )
            return new ExplosionAction(coord, targetInfo->explosionGFX);
        else
            return new DamageAction(coord);
    }
    else
    {
        direction = atan2(coord.y - target->getPosition().y, coord.x - target->getPosition().x);

        float speed = std::min( dist, 14.0f );

        coord.x -= speed*cos(direction);
        coord.y -= speed*sin(direction);
    }

    ++frame;
    if (frame > shootEffect->getNumberFrames())
        frame = 0;

    return 0;
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
    shootEffect->DrawImage((int) (coord.x ), (int) (coord.y ), frame, ( (direction+PI) * 180.0) /PI, Game::getGlobalGame()->getRenderer());
}

KamikazeAction::KamikazeAction(Ship *Source, Ship *Target, Image *imgShoot)
    : source(Source), target(Target), coord(Source->getPosition()), distance(0),explosionEffect(imgShoot)
{
}

Action* KamikazeAction::act()
{
    if (!target->isAlive())
    {
        complete = 1;
        source->kill();
    }

    float dist = source->getPosition().distance(target->getPosition());
    if ( dist < 20.0f )
    {
    	source->kill();

        target->takeDamage(source->getBaseStats().damage * distance/500);

//        printf("%p kamikaze to %p for %f damage\n", source, target, source->getBaseStats().damage * distance/400);
        complete = 1;
    }
    else
    {
        float direction = atan2(source->getY() - target->getPosition().y, source->getX() - target->getPosition().x);

        // Causar dano a nave kamikaze [NERF]
        source->takeDamage( std::max( source->getHP()/100, 0.3) );

        float speed = std::min( dist, 14.0f );
        distance += speed;

        source->move(-speed*cos(direction), -speed*sin(direction));
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
