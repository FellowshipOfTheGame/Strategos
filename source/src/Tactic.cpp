/*
 *  Tactic.cpp
 *  Strategos
 *
 *  Created by Guilherme S. Gibertoni on 7/6/12.
 *  Copyright 2012 Tinker Entertainment. All rights reserved.
 *
 */

#include <stdio.h>

#include "Global.h"

#include "Tactic.h"
#include "Unit.h"

Tactic* Tactic::copy(const Tactic *source)
{
    Tactic *t_copy = nullptr;

    switch(source->type)
    {
        case 0:
            t_copy = new AttackNearestEnemy(source->info, source->tacticTrigger);
            break;

        case 1:
            t_copy = new AttackWeakestEnemy(source->info, source->tacticTrigger);
            break;

        case 2:
            t_copy = new AttackCollab(source->info, source->tacticTrigger);
            break;

        case 3:
            t_copy = new DefenseCollab(source->info, source->tacticTrigger);
            break;

        case 4:
            t_copy = new Kamikase(source->info, source->tacticTrigger);
            break;

        case 5:
            t_copy = new Retreat(source->info, source->tacticTrigger);
            break;

        default:
            break;
    }

    return t_copy;
}

AttackNearestEnemy::AttackNearestEnemy(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_NEAREST_ENEMY)
{

}

int AttackNearestEnemy::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	if (tvd.enemyUnits.size() == 0 || tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	tvd.validatingUnit->setTarget(-1);

	int Ret = 0;
	Coordinates myPos = tvd.validatingUnit->getAveragePos();
	float minDist = 99999;

	Unit *nearestUnit = NULL;
	for (unsigned int i = 0; i < tvd.enemyUnits.size(); ++i)
	{
		Coordinates enemyAvrg = tvd.enemyUnits[i]->getAveragePos();
		if (tvd.enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < minDist)
			{
				minDist = dist;
				nearestUnit = tvd.enemyUnits[i];
				tvd.validatingUnit->setTarget(i);
			}
		}
	}

	if (nearestUnit && minDist < tvd.validatingUnit->getSquadBaseStats().range)
	{
		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % nearestUnit->nShips();
					if (nearestUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, nearestUnit->getShip(s), tvd.validatingUnit->getUnitInfo(), nearestUnit->getUnitInfo()));
						++Ret;
						break;
					}
				}
			}
		}
	}

    // TODO: Se eu nao conseguir atacar, devo desconsiderar o target?
//	if (Ret == 0)
//        squad->setTarget(-1);

	return Ret;
}

// ---

AttackWeakestEnemy::AttackWeakestEnemy(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_WEAKEST_ENEMY)
{

}

int AttackWeakestEnemy::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	tvd.validatingUnit->setTarget(-1);
	if (tvd.enemyUnits.size() == 0)
	{
		return 0;
	}

	int Ret = 0;
	float minHP = 100;
	Coordinates enemyAvrg;
	int sumHP;

	Coordinates myPos = tvd.validatingUnit->getAveragePos();
	if (tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	Unit *wekeastUnit = NULL;

	//cria uma lista com a unidades dentro do alcançe
	for (unsigned int i = 0; i < tvd.enemyUnits.size(); i++)
	{
		Coordinates enemyAvrg = tvd.enemyUnits[i]->getAveragePos();
		if (tvd.enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < tvd.validatingUnit->getSquadBaseStats().range)
			{
			    // Verificar HP desta unidade
                sumHP = 0;
                // TODO: Colocar isso na Unit
                for (unsigned int j = 0; j < tvd.enemyUnits[i]->nShips(); ++j){
                    sumHP += tvd.enemyUnits[i]->getShip(j)->getHP();
                }

                int maxHP = tvd.enemyUnits[i]->getUnitInfo()->squadSize * tvd.enemyUnits[i]->getUnitInfo()->stats.maxHP;

                // Considerar apenas o inteiro
                int percent = (sumHP / maxHP) * 100;
                if (percent < minHP)
                {
                    minHP = percent;
                    wekeastUnit = tvd.enemyUnits[i];
                    tvd.validatingUnit->setTarget( i );
                }
			}
		}
	}

	//ataca as naves do esquadrao de maneira aleatoria
	if (wekeastUnit)
	{
		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % wekeastUnit->nShips();
					if (wekeastUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, wekeastUnit->getShip(s), tvd.validatingUnit->getUnitInfo(), wekeastUnit->getUnitInfo()));
						++Ret;
						break;
					}
				}
			}
		}
	}

    // TODO: Se eu nao conseguir atacar, devo desconsiderar o target?
//	if (Ret == 0)
//        squad->setTarget(-1);

	return Ret;
}

// ---
AttackCollab::AttackCollab(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_COLLAB)
{
}

int AttackCollab::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	tvd.validatingUnit->setTarget(-1);

    // TODO: Algumas unidades chegam aqui com aliados invalidos! arrumar!
	if (info.allyUnitID >= tvd.alliedUnits.size())
        return 0;

	Unit *allyUnit = tvd.alliedUnits[info.allyUnitID];
	if (allyUnit->getTarget() == -1)
        return 0;

    int target = allyUnit->getTarget();

    if (target >= tvd.enemyUnits.size() || target < 0)
        return 0;

	tvd.validatingUnit->setTarget(target);

	Unit *enemyUnit = tvd.enemyUnits[target];
	if (enemyUnit->getNShipsAlive() == 0){
		return 0;
	}

	/// TODO: Adicionar restricao de distancia?

    float dist = tvd.validatingUnit->getAveragePos().distance(enemyUnit->getAveragePos());
    if (dist < tvd.validatingUnit->getUnitInfo()->stats.range)
    {
        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % enemyUnit->nShips();
					if (enemyUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, enemyUnit->getShip(s), tvd.validatingUnit->getUnitInfo(), enemyUnit->getUnitInfo()));
						break;
					}
				}
			}
		}
    }
    else
    {
        Coordinates coord = tvd.validatingUnit->getAveragePos();
        float direction = atan2(enemyUnit->getAveragePos().y - coord.y, enemyUnit->getAveragePos().x - coord.x);

        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
		    Ship *iShip = tvd.validatingUnit->getShip(i);

            if (iShip->isAlive() && iShip->getStats().isMoving != move_action)
            {
                coord = iShip->getPosition();
                coord.x += cos(direction)*SPACIAL_UNIT;
                coord.y += sin(direction)*SPACIAL_UNIT;

                newActions.push_back(new MoveAction( iShip, coord, true ) );
            }
		}
    }

	return 1;
}

string AttackCollab::printTactic()
{
	string str;

	str = Tactic::printTactic();

	str.append(" ");
	str.append(std::to_string(info.allyUnitID));

	return str;
}

// ---
DefenseCollab::DefenseCollab(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_DEFENSE_COLAB)
{

}

int DefenseCollab::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	int Ret = 0;
	Unit *enemyUnit = NULL;

	for (unsigned int j = 0; j < tvd.enemyUnits.size(); j++)
	{
		// Interrompe caso tenha encontrado um inimigo para atacar
		if (tvd.enemyUnits[j]->getTarget() == info.allyUnitID)
		{
			enemyUnit = tvd.enemyUnits[j];
			tvd.validatingUnit->setTarget(j);
			break;
		}
	}

	if (enemyUnit == NULL || enemyUnit->getNShipsAlive() == 0)
		return 0;

    // Para cada ship da unidade
	for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); i++)
	{
		Ship *iShip = tvd.validatingUnit->getShip(i);
		// Encontrar alvo
		if ( iShip->isAlive()
        && ( iShip->getStats().currentAtkCD == 0 || iShip->getStats().isMoving != move_action ) )
		{
			Coordinates shipPos = iShip->getPosition();
			Ship *nearestShip = NULL;
			float dist_ = 99999;

            // Verificar ships inimigas
			for (unsigned int u = 0; u < enemyUnit->nShips(); ++u)
			{
				Ship *eShip = enemyUnit->getShip(u);

				if (eShip->isAlive())
				{
					float d = shipPos.distance(eShip->getPosition());
					if (d < dist_)
					{
						nearestShip = eShip;
						dist_ = d;
					}
				}
			}

            if (nearestShip)
            {
                if (iShip->getStats().currentAtkCD == 0 && dist_ < iShip->getBaseStats().range)
                {
                    iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
                    newActions.push_back(new AttackAction(iShip, nearestShip, tvd.validatingUnit->getUnitInfo(), enemyUnit->getUnitInfo()));
                    ++Ret;
                }
                else if (iShip->getStats().isMoving != move_action)
                {
                    newActions.push_back( new MoveAction(iShip, nearestShip, true) );
                    ++Ret;
                }
            }
		}
	}

	return Ret;
}

string DefenseCollab::printTactic()
{
	string str;

	str = Tactic::printTactic();

	str.append(" ");
	str.append(std::to_string(info.allyUnitID));

	return str;
}

// ---

Kamikase::Kamikase(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_KAMIKASE)
{

}

int Kamikase::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	if (tvd.enemyUnits.size() == 0)
		return 0;

	int Ret = 0;
	float minDist = 99999;

	Coordinates myPos = tvd.validatingUnit->getAveragePos();
	if (tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	Unit *nearestUnit = NULL;
	for (unsigned int i = 0; i < tvd.enemyUnits.size(); ++i)
	{
		Coordinates enemyAvrg = tvd.enemyUnits[i]->getAveragePos();
		if (tvd.enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < minDist)
			{
				minDist = dist;
				nearestUnit = tvd.enemyUnits[i];
				tvd.validatingUnit->setTarget(i);
			}
		}
	}

	if (nearestUnit && minDist < tvd.validatingUnit->getSquadBaseStats().range)
	{
		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().isKamikasing == false)
			{
				while (1)
				{
					int s = rand()%nearestUnit->nShips();
					if (nearestUnit->getShip(s)->isAlive())
					{
						iShip->getStats().isKamikasing = true;
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD * 99;
						newActions.push_back(new KamikazeAction(iShip, nearestUnit->getShip(s), NULL));
						++Ret;
						break;
					}
				}
			}
		}
	}

	return Ret;
}

// ---

Retreat::Retreat(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_RETREAT)
{
}

int Retreat::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	Coordinates myPos, motherAvrg, enemyAvrg;

	Unit *motherShipUnit = NULL;
	for (unsigned int i = 0; i < tvd.alliedUnits.size(); ++i)
	{
		if (tvd.alliedUnits[i]->getType() == 0)
		{
			motherShipUnit = tvd.alliedUnits[i];
			break;
		}
	}

	motherAvrg = motherShipUnit->getAveragePos();
	myPos = tvd.validatingUnit->getAveragePos();

	if (myPos.distance(motherAvrg) > tvd.validatingUnit->getSquadBaseStats().range)
	{
	    motherAvrg = motherShipUnit->getAveragePos();
        motherAvrg.x += rand() % 300 - rand() % 300;
        motherAvrg.y += rand() % 300 - rand() % 300;

		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); i++)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);
			if (iShip->isAlive() && iShip->getStats().isMoving != move_action)
			{
                Coordinates rndCoord((rand() % 128) - (rand() % 128), (rand() % 128) - (rand() % 128));
                newActions.push_back(new MoveAction(iShip, motherAvrg + rndCoord));
			}
		}
	}

	return 1;
}

string Retreat::printTactic()
{
	string str;

	str = Tactic::printTactic();

	str.append(" ");
	str.append(std::to_string(info.allyUnitID)); // TODO: info.allyUnitID Not used for Retreat?

	return str;
}

// ---

MoveRandomly::MoveRandomly(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_MOVE_RANDOM)
{
	cooldown = 0;
}

int MoveRandomly::validateTactic(list<Action*> &newActions, TacticValidationData& tvd)
{
	if (cooldown > 0)
	{
		--cooldown;
		return 0;
	}

    // TODO: Tamanho da tela de combate? Considerar lado da tela
	Coordinates coordBase = tvd.validatingUnit->getAveragePos();
	coordBase.x += rand() % 200 - rand() % 200;
	coordBase.y += rand() % 200 - rand() % 200;

	for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
	{
	    if (tvd.validatingUnit->getShip(i)->getStats().isMoving != move_action)
        {
            Coordinates rndCoord((rand()%64) - (rand()%64), (rand()%64) - (rand()%64));
            newActions.push_back(new MoveAction(tvd.validatingUnit->getShip(i), coordBase + rndCoord));
	    }
	}

	cooldown = (rand() % 90) + 30;

	return 1;
}
