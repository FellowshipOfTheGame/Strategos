/*
 *  Strategos - Unit.cpp
 *
 */

#include "Global.h"
#include "Unit.h"
#include "Random.h"
#include "Ship.h"

#if defined(WIN32)
    #include "SDL2_gfx\SDL2_gfxPrimitives.h"
#elif defined(__APPLE__)
    #include "SDL2_gfx/SDL2_gfxPrimitives.h"
#endif // _MAC_OS_

static int max_tatics_per_unit = 0;

void print_MaxActions()
{
    printf("Limit Actions: %d\n", max_tatics_per_unit);
    max_tatics_per_unit = 0;
}

Unit::Unit(unsigned long ID, const DictKey *info, Coordinates position)
    : id(ID), mySquadInfo(info), bluePrintCoord(position),
    target(-1), baseCoord(position), averageCoord(position),
        basicTacticMoveRandom( TacticInfo(0), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) ),
        basicTacticAttackNearest( TacticInfo(0), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) ),
        basicTacticRetreat( TacticInfo(0), TacticTrigger(0, 0, TRIGGER_LOGIC_OR) )
{
	restoreUnit(0);
}

Unit::Unit(const Unit* copy)
    :   id(copy->id), mySquadInfo(copy->mySquadInfo), bluePrintCoord(copy->bluePrintCoord),
        target(-1), baseCoord(bluePrintCoord), averageCoord(bluePrintCoord),
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
	for (int i = 0; i < tactics.size(); ++i){
        delete tactics[i];
    }

    for (int i = 0; i < ships.size(); ++i){
        delete ships[i];
    }

	for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
        delete *it;
}

void Unit::restoreUnit(int teamID, const Coordinates atBaseP)
{
    for (unsigned int i = 0; i < ships.size(); ++i)
        delete ships[i];
    ships.clear();

    for (std::list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
        delete *it;
    shipsActions.clear();

    baseCoord = atBaseP;
    averageCoord = atBaseP;

    double circleRadius = 12.0 * (mySquadInfo->squadSize-1);
    float dAngle = 6.283185306 / mySquadInfo->squadSize;
	for (unsigned int i = 0; i < mySquadInfo->squadSize; i++)
	{
		Coordinates coordships(atBaseP);
		coordships.x += circleRadius * cos(i * dAngle);
		coordships.y += -circleRadius * sin(i * dAngle);
		Ship *nship = new Ship(mySquadInfo->stats, coordships);

		ships.push_back(nship);
	}

	shipsAlive = mySquadInfo->squadSize;
	target = -1;
	team = teamID;
}

int Unit::getTeam() const
{
    return team;
}

void Unit::restoreUnit(int teamID)
{
    restoreUnit(teamID, bluePrintCoord);
}

void Unit::addTactic(Tactic *tactic)
{
	tactics.push_back(tactic);
}

void Unit::removeTactic(int n)
{
	int i = 0;
	vector<Tactic*>::iterator iter = tactics.begin();
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

void Unit::setTarget(int i)
{
	target = i;
}

int Unit::getTarget()
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
	if (pos < tactics.size())
		return tactics[pos];
	return NULL;
}

unsigned int Unit::getTacticSize(){
	return tactics.size();
}

Ship* Unit::getShip(unsigned long gid)
{
    if (gid < ships.size())
        return ships[gid];

    return nullptr;
}

unsigned long Unit::getID() const{
	return id;
}

void Unit::setID(int id){
	this->id = id;
}

int Unit::getType(){
	return mySquadInfo->type;
}

float Unit::getBaseX(){
	return baseCoord.x;
}

float Unit::getBaseY(){
	return baseCoord.y;
}

void Unit::setBluePrintCoord(const Coordinates& coord){
    bluePrintCoord = coord;
}

const Coordinates& Unit::getBluePrintCoord(){
	return bluePrintCoord;
}

float Unit::getBluePrintX(){
	return bluePrintCoord.x;
}

float Unit::getBluePrintY(){
	return bluePrintCoord.y;
}

float Unit::getAvgX(){
	return averageCoord.x;
}

float Unit::getAvgY(){
	return averageCoord.y;
}

const Coordinates& Unit::getBaseCoord(){
	return baseCoord;
}

const shipBaseStats& Unit::getSquadBaseStats(){
	return mySquadInfo->stats;
}

int Unit::getNShipsAlive(){
    return shipsAlive;
}

const Coordinates& Unit::getAveragePos() const{
    return averageCoord;
}

bool Unit::hover(float camOX, float camOY)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	if (averageCoord.distance(mouseX + camOX, mouseY + camOY) < 64)
	{
		return true;
	}

	return false;
}

unsigned long Unit::nShips() const
{
    return mySquadInfo->squadSize;
}

void Unit::updateActions()
{
	for (list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
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

int Unit::update()
{
    // Atualizar numero de naves vivas e posicao media
    shipsAlive = 0;
    Coordinates totalPos(0, 0);
	for (unsigned int i = 0; i < ships.size(); i++)
	{
		if (ships[i]->isAlive())
		{
			ships[i]->update();
			totalPos += ships[i]->getPosition();
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
	printf("maxHP: %lf\n", mySquadInfo->stats.maxHP);
	printf("damage: %lf\n", mySquadInfo->stats.damage);
	printf("speed: %lf\n", mySquadInfo->stats.speed);
	printf("range: %lf\n", mySquadInfo->stats.range);
	printf("shield: %lf\n", mySquadInfo->stats.shield);
	printf("dodge: %lf%%\n", mySquadInfo->stats.dodge);
	printf("atkCooldown: %d\n", mySquadInfo->stats.maxAtkCD);
	printf("squadSize: %d\n", mySquadInfo->squadSize);

	for (unsigned int i = 0; i < tactics.size(); i++)
		tactics[i]->printTactic();
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

void Unit::moveTo(Coordinates c)
{
	for (unsigned int i = 0; i < nShips(); ++i)
	{
		if (ships[i]->isAlive())
		{
			Coordinates rndCoord((rand() % 128) - (rand() % 128), (rand() % 128) - (rand() % 128));
			rndCoord += c;
			ships[i]->moveTo(c);
		}
	}

}

void Unit::render()
{
	for (list<Action*>::iterator it = shipsActions.begin(); it != shipsActions.end(); ++it)
	{
		(*it)->render();
	}

	// Printar todas as direcoes da nave DEBUG
	/*for(unsigned int j = 0; j < _ROTATION_FRAMES_; j++)
	 {
	 Image *img = mySquadInfo->shipsGFX[j];
	 img->DrawImage(j * 32, mySquadInfo->type * 34, Game::getGlobalGame()->getScreenSurface());
	 }*/

    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();

    if (!mySquadInfo->shipsGFX){
			printf("No img");
    }else{
        for (unsigned int j = 0; j < nShips(); j++)
        {
            Ship *ship = ships[j];

            if (!ship->isAlive()) continue;

            float x = ship->getDirection() * 180 / PI;
            if (x < 0)
                x += 360;

            while (x < 0.0) x += 360.0;
            while (x > 360.0) x -= 360.0;

            int frame = ((int(x) % 360) / (360 / _ROTATION_FRAMES_)) % _ROTATION_FRAMES_;
            Image *img = mySquadInfo->shipsGFX[frame];

//			circleRGBA(Game::getGlobalGame()->getScreenSurface(), ship->getX() - cOffX, ship->getY() - cOffY, mySquadInfo->stats.range, 0, 0, 255, 40);
            if (img){
                img->DrawImage(ship->getX() - (img->getFrameWidth() / 2), ship->getY() - (img->getFrameHeight() / 2),
                        renderer);
            }else{
//                filledCircleRGBA(Game::getGlobalGame()->getRenderer(),
//                           ship->getX() - cOffX, ship->getY() - cOffY,
//                           mySquadInfo->stats.range, 255, 0, 0, 128);
            }

            SDL_Rect rLife;
            rLife.x = ship->getX();
            rLife.y = ship->getY();
            rLife.w = ship->getHP()/20;
            rLife.h = 2;

            // Barra de vida
            SDL_SetRenderDrawColor(renderer, 0,255,0, 200);
            SDL_RenderFillRect(renderer, &rLife);
//            for (int i = 0; i < 5; ++i){
//                arcRGBA(Game::getGlobalGame()->getRenderer(), ship->getX() - (img->getFrameWidth() / 2) - cOffX, ship->getY() - (img->getFrameHeight() / 2) - cOffY,
//                         30-i, 180, 360, 255, 255, 255, 255);
//            }

            // Barra de Shield
            if (ship->getStats().currentShield > 0){
                SDL_SetRenderDrawColor(renderer, 228,228,228, 250);
                rLife.w = ship->getStats().currentShield/20;
                rLife.y += 2;
                SDL_RenderFillRect(renderer, &rLife);
            }

            SDL_RenderDrawLine(renderer, ship->getPosition().x, ship->getPosition().y, ship->getTargetPos().x, ship->getTargetPos().y );

            SDL_Rect a;
            a.x = ship->getPosition().x;
            a.y = ship->getPosition().y;
            a.w = 5;
            a.h = 5;
            if (ship->getMoving() == move_not_moving){
                SDL_SetRenderDrawColor(renderer, 255, 0,0 ,255);
            }
            else if (ship->getMoving() == move_action){
                SDL_SetRenderDrawColor(renderer, 0, 255, 0 ,255);
            }
            else{
                SDL_SetRenderDrawColor(renderer, 0, 0,255 ,255);
            }
            SDL_RenderDrawRect(renderer, &a );

            if (ship->getStats().isKamikasing)
            {
                a.x -=5;
                a.y -=5;
                a.w += 10;
                a.h += 10;
                SDL_SetRenderDrawColor(renderer, 0, 255,255 ,255);
                SDL_RenderDrawRect(renderer, &a );
            }
        }
    }

	circleRGBA(renderer, averageCoord.x, averageCoord.y, 64, 0, 128, 0, 128);
}

