/*
 *  Strategos - Unit.cpp
 *
 */

#include "Global.h"
#include "Game.h"

#include "Unit.h"
#include "Ship.h"

#include "SDL2_gfx/SDL2_gfxPrimitives.h"

#define LIFE_BAR_SIZE   70

static int max_tatics_per_unit = 0;

void print_MaxActions()
{
    printf("Limit Actions: %d\n", max_tatics_per_unit);
    max_tatics_per_unit = 0;
}

Unit::Unit(unsigned int ID, const DictKey *info, Coordinates position)
    : bluePrintCoord(position), mySquadInfo(info), id(ID), team(0), myLog(nullptr),
        target(nullptr), baseCoord(position), averageCoord(position),
        basicTacticMoveRandom( TacticInfo(nullptr), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) ),
        basicTacticAttackNearest( TacticInfo(nullptr), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) ),
        basicTacticRetreat( TacticInfo(nullptr), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) )
{
	restoreUnit(0);
}

Unit::Unit(const Unit* copy)
    :   bluePrintCoord(copy->bluePrintCoord), mySquadInfo(copy->mySquadInfo),
        id(copy->id), team(0), myLog(nullptr),
        target(nullptr), baseCoord(bluePrintCoord), averageCoord(bluePrintCoord),
        basicTacticMoveRandom( copy->basicTacticMoveRandom ),
        basicTacticAttackNearest( copy->basicTacticAttackNearest ),
        basicTacticRetreat( copy->basicTacticRetreat )
{
    // Copiar todos os dados
    // Efetuar copia dos objetos dos ponteiros
    for (unsigned int i = 0; i < copy->tactics.size(); ++i)
        tactics.push_back(Tactic::copy(copy->tactics[i]));

    restoreUnit(copy->team);
}

Unit::~Unit()
{
	for (unsigned int i = 0; i < tactics.size(); ++i){
        delete tactics[i];
    }

    for (unsigned int i = 0; i < ships.size(); ++i){
        delete ships[i];
    }

	for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
        delete *it;
}

void Unit::restoreUnit(int teamID, const Coordinates atBaseP, CombatLog *log)
{
    for (unsigned int i = 0; i < ships.size(); ++i)
        delete ships[i];
    ships.clear();

    for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
        delete *it;
    shipsActions.clear();

    baseCoord = atBaseP;
    averageCoord = atBaseP;

    if (log)
        myLog = log->getLogForUnit(id);
    else
        myLog = nullptr;

    double circleRadius = 12.0 * (mySquadInfo->squadSize-1);
    float dAngle = 6.283185306 / mySquadInfo->squadSize;
	for (unsigned int i = 0; i < mySquadInfo->squadSize; i++)
	{
		Coordinates coordships(atBaseP);
		coordships.x += circleRadius * cos(i * dAngle);
		coordships.y += -circleRadius * sin(i * dAngle);
		Ship *nship = new Ship(mySquadInfo->stats, coordships, myLog);

		ships.push_back(nship);
	}

	shipsAlive = mySquadInfo->squadSize;
	shipsHP = mySquadInfo->squadSize*mySquadInfo->stats.maxHP;
	target = nullptr;
	team = teamID;

}

int Unit::getTeam() const
{
    return team;
}

void Unit::restoreUnit(int teamID, CombatLog *log)
{
    restoreUnit(teamID, bluePrintCoord, log);
}

void Unit::addTactic(Tactic *tactic)
{
	tactics.push_back(tactic);
}

void Unit::removeTactic(int n)
{
	int i = 0;
	std::vector<Tactic*>::iterator iter = tactics.begin();
	while (iter != tactics.end())
	{
		if (i == n)
		{
//		    delete *(tactics.begin() + i) // Checar isso
			iter = tactics.erase(tactics.begin() + i);

			break;
		}
		i++;
	}
}

void Unit::setTarget(Unit *unit)
{
	target = unit;
}

Unit* Unit::getTarget() const
{
	return target;
}

void Unit::setTacticAt(Tactic *tactic, int pos)
{
	printf ("tentando dar insert em %d\n",pos);
	tactics.insert(tactics.begin()+pos, tactic);
	removeTactic(pos+1);
}

Tactic* Unit::getTacticAt(unsigned int pos)
{
    return tactics[pos];
}

unsigned int Unit::getTacticSize() const{
	return tactics.size();
}

Ship* Unit::getShip(unsigned long gid) const
{
    return ships[gid];
}

unsigned int Unit::getID() const{
	return id;
}

void Unit::setID(int id){
	this->id = id;
}

int Unit::getType() const{
	return mySquadInfo->type;
}

float Unit::getBaseX() const{
	return baseCoord.x;
}

float Unit::getBaseY() const{
	return baseCoord.y;
}

void Unit::setBluePrintCoord(const Coordinates& coord){
    bluePrintCoord = coord;
}

const Coordinates& Unit::getBluePrintCoord() const{
	return bluePrintCoord;
}

float Unit::getBluePrintX() const{
	return bluePrintCoord.x;
}

float Unit::getBluePrintY() const{
	return bluePrintCoord.y;
}

float Unit::getAvgX() const{
	return averageCoord.x;
}

float Unit::getAvgY() const{
	return averageCoord.y;
}

const Coordinates& Unit::getBaseCoord() const{
	return baseCoord;
}

const shipBaseStats& Unit::getSquadBaseStats() const{
	return mySquadInfo->stats;
}

int Unit::getNShipsAlive() const{
    return shipsAlive;
}

const Coordinates& Unit::getAveragePos() const{
    return averageCoord;
}

bool Unit::hover(float mX, float mY) const
{
	return (averageCoord.distance(mX, mY) < 64);
}

unsigned long Unit::nShips() const
{
    return mySquadInfo->squadSize;
}

float Unit::getHPsum() const
{
    return shipsHP;
}

void Unit::updateActions()
{
	for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
	{
		if ((*it)->completed())
		{
			delete *it;
			it = shipsActions.erase(it);
		}
		else
		{
			Action* reaction = (*it)->act();
			if (reaction){
                shipsActions.push_back(reaction);
            }
		}
	}
}

int Unit::update(RandomEngine& randE)
{
    // Atualizar numero de naves vivas e posicao media
    shipsAlive = 0;
    shipsHP = 0.0;
    Coordinates totalPos(0, 0);
	for (unsigned int i = 0; i < ships.size(); i++)
	{
		if (ships[i]->isAlive())
		{
			ships[i]->update(randE);
			totalPos += ships[i]->getPosition();
			shipsHP += ships[i]->getHP();
			++shipsAlive;
//            printf("-> HP: %lf, (%d, %d)\n", ships[i]->getHP(), ships[i]->getX(), ships[i]->getY());
		}
	}
	totalPos.x /= shipsAlive;
	totalPos.y /= shipsAlive;
	averageCoord = totalPos;

//	if (max_tatics_per_unit < shipsActions.size()){
//        max_tatics_per_unit = shipsActions.size();
//
//        int EA = 0;
//        int DA = 0;
//        int MA = 0;
//        int AA = 0;
//        int KA = 0;
//
//        for (Action* ac : shipsActions)
//        {
//            if (dynamic_cast<ExplosionAction*>(ac))
//                EA++;
//            else if (dynamic_cast<DamageAction*>(ac))
//                DA++;
//            else if (dynamic_cast<MoveAction*>(ac))
//                MA++;
//            else if (dynamic_cast<AttackAction*>(ac))
//                AA++;
//            else if (dynamic_cast<KamikazeAction*>(ac))
//                KA++;
//        }
//
//        printf("ExpAc: %d\n", EA);
//        printf("DmgAc: %d\n", DA);
//        printf("MovAc: %d\n", MA);
//        printf("AtkAc: %d\n", AA);
//        printf("KamAc: %d\n\n", KA);
//
//	}

	return shipsAlive;
}

void Unit::printInfo()
{
    printf("\nUNIT: %d\n", id);
	printf("maxHP: %lf\n", mySquadInfo->stats.maxHP);
	printf("damage: %lf\n", mySquadInfo->stats.damage);
	printf("speed: %lf\n", mySquadInfo->stats.speed);
	printf("range: %lf\n", mySquadInfo->stats.range);
	printf("shield: %lf\n", mySquadInfo->stats.shield);
	printf("dodge: %lf%%\n", mySquadInfo->stats.dodge);
	printf("atkCooldown: %d\n", mySquadInfo->stats.maxAtkCD);
	printf("squadSize: %d \t Alive: %d\n", mySquadInfo->squadSize, shipsAlive);

    printf("Tactics: %d\n", tactics.size());
	for (unsigned int i = 0; i < tactics.size(); i++)
		printf("-> %s\n",tactics[i]->printTactic().c_str());

    printf("Actions: %d\n", shipsActions.size());
    for (Action* ac : shipsActions)
    {
        if (dynamic_cast<ExplosionAction*>(ac))
            printf("-> Explosion Action\n");
        else if (dynamic_cast<DamageAction*>(ac))
            printf("-> Damage Action\n");
        else if (dynamic_cast<MoveAction*>(ac))
            printf("-> Move Action\n");
        else if (dynamic_cast<AttackAction*>(ac))
            printf("-> Attack Action\n");
        else if (dynamic_cast<KamikazeAction*>(ac))
            printf("-> Kamikaze Action\n");
    }
}

const DictKey* Unit::getUnitInfo() const
{
	return mySquadInfo;
}

void Unit::generateActions(TacticValidationData& tvd)
{
    tvd.validatingUnit = this;

	for (unsigned int i = 0; i < tactics.size(); i++)
	{
		if (tactics[i]->testTrigger(this))
		{
			if (tactics[i]->validateTactic(shipsActions, tvd) != 0){
				return;
			}
		}
	}

	// Validar taticas basicas
	basicTacticMoveRandom.validateTactic(shipsActions, tvd);
	basicTacticAttackNearest.validateTactic(shipsActions, tvd);
	if (this->mySquadInfo->type != 0)
		basicTacticRetreat.validateTactic(shipsActions, tvd);
}

void Unit::moveTo(const Coordinates& c)
{
	for (unsigned int i = 0; i < nShips(); ++i)
	{
		if (ships[i]->isAlive())
		{
			ships[i]->moveTo(c);
		}
	}

}

void Unit::render()
{
	for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
	{
		(*it)->render();
	}

    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();

    if (!mySquadInfo->shipsGFX){
			printf("No img");
    }else{
        for (unsigned int j = 0; j < nShips(); j++)
        {
            Ship *ship = ships[j];

            if (!ship->isAlive()) continue;

            float x = ship->getDirection() * 180.0 / M_PI;
            while (x < 0.0)     x += 360.0;

            int frame = ((int(x) % 360) / (360 / _ROTATION_FRAMES_)) % _ROTATION_FRAMES_;
            Image *img = mySquadInfo->shipsGFX[frame];

            if (img){
                img->DrawImage(renderer, ship->getX() - (img->getFrameWidth() / 2), ship->getY() - (img->getFrameHeight() / 2));
            }

            SDL_Rect rLife;
            rLife.x = ship->getX() - LIFE_BAR_SIZE/2;
            rLife.y = ship->getY() - 32;
            rLife.h = 4;

            // Barra de vida fundo
            SDL_SetRenderDrawColor(renderer, 0,0,0, 200);
            rLife.w = LIFE_BAR_SIZE;
            SDL_RenderFillRect(renderer, &rLife);

            // Interior
            rLife.x += 1;
            rLife.y += 1;
            rLife.h -= 2;
            rLife.w = (ship->getHP()/ship->getBaseStats().maxHP)*LIFE_BAR_SIZE-2;
            SDL_SetRenderDrawColor(renderer,
                                   255*(1.0-ship->getHP()/ship->getBaseStats().maxHP),
                                   255*(ship->getHP()/ship->getBaseStats().maxHP), 0, 200);
            SDL_RenderFillRect(renderer, &rLife);

            // Barra de Shield
            if (ship->getStats().currentShield > 0)
            {
                SDL_SetRenderDrawColor(renderer, 228,228,228, 200);
                rLife.w = ship->getStats().currentShield/ship->getBaseStats().shield*LIFE_BAR_SIZE-2;
                SDL_RenderFillRect(renderer, &rLife);
            }

            // Target line
            SDL_RenderDrawLine(renderer, ship->getPosition().x, ship->getPosition().y,
                               ship->getTargetPos().x, ship->getTargetPos().y );

            // Debug
            SDL_Rect a;
            a.x = ship->getPosition().x;
            a.y = ship->getPosition().y;
            a.w = 5;
            a.h = 5;
            if (ship->getMoving() == move_not_moving){
                SDL_SetRenderDrawColor(renderer, 255, 0,0 ,255);
            }else if (ship->getMoving() == move_action){
                SDL_SetRenderDrawColor(renderer, 0, 255, 0 ,255);
            }else{
                SDL_SetRenderDrawColor(renderer, 0, 0,255 ,255);
            }
            SDL_RenderFillRect(renderer, &a );

            if (ship->getStats().isKamikasing)
            {
                a.x -=5;
                a.y -=5;
                a.w += 10;
                a.h += 10;
                SDL_SetRenderDrawColor(renderer, 0, 255,255 ,255);
                SDL_RenderFillRect(renderer, &a );
            }
        }
    }

    // Desenhar circulo do time
    if (team == 0)
        circleRGBA(renderer, averageCoord.x, averageCoord.y, 64, 255,0,0, 240);
    else
        circleRGBA(renderer, averageCoord.x, averageCoord.y, 64, 0,0,255, 240);
}

