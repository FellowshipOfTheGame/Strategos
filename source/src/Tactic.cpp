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
    Tactic *copy = nullptr;

    switch(source->type)
    {
        case 0:
            copy = new AttackNearestEnemy(source->info, source->tacticTrigger);
            break;

        case 1:
            copy = new AttackWeakestEnemy(source->info, source->tacticTrigger);
            break;

        case 2:
            copy = new AttackCollab(source->info, source->tacticTrigger);
            break;

        case 3:
            copy = new DefenseCollab(source->info, source->tacticTrigger);
            break;

        case 4:
            copy = new Kamikase(source->info, source->tacticTrigger);
            break;

        case 5:
            copy = new Retreat(source->info, source->tacticTrigger);
            break;

        default:
            break;
    }

    return copy;
}

AttackNearestEnemy::AttackNearestEnemy(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_NEAREST_ENEMY)
{

}

int AttackNearestEnemy::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits,
        const vector<Unit*>& alliedUnits)
{
	if (enemyUnits.size() == 0 || squad->getNShipsAlive() == 0)
		return 0;

	squad->setTarget(-1);

	int Ret = 0;
	Coordinates myPos = squad->getAveragePos();
	float minDist = 99999;

	Unit *nearestUnit = NULL;
	for (unsigned int i = 0; i < enemyUnits.size(); ++i)
	{
		Coordinates enemyAvrg = enemyUnits[i]->getAveragePos();
		if (enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < minDist)
			{
				minDist = dist;
				nearestUnit = enemyUnits[i];
				squad->setTarget(i);
			}
		}
	}

	if (nearestUnit && minDist < squad->getSquadBaseStats().range)
	{
		for (unsigned int i = 0; i < squad->nShips(); ++i)
		{
			Ship *iShip = squad->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % nearestUnit->nShips();
					if (nearestUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, nearestUnit->getShip(s), squad->getUnitInfo(), nearestUnit->getUnitInfo()));
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

AttackWeakestEnemy::AttackWeakestEnemy(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_WEAKEST_ENEMY)
{

}

int AttackWeakestEnemy::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits,
        const vector<Unit*>& alliedUnits)
{
	squad->setTarget(-1);
	if (enemyUnits.size() == 0)
	{
		return 0;
	}

	int Ret = 0;
	float minHP = 100;
	Coordinates enemyAvrg;
	int sumHP;

	Coordinates myPos = squad->getAveragePos();
	if (squad->getNShipsAlive() == 0)
		return 0;

	Unit *wekeastUnit = NULL;

	vector<Unit*> enemyNear;
	//cria uma lista com a unidades dentro do alcançe
	for (unsigned int i = 0; i < enemyUnits.size(); i++)
	{
		Coordinates enemyAvrg = enemyUnits[i]->getAveragePos();
		if (enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < squad->getSquadBaseStats().range)
			{
				enemyNear.push_back(enemyUnits[i]);
			}
		}
	}

	//percorre a lista de unidades inimigas no alçance um busca da mais fraca
	for (unsigned int i = 0; i < enemyNear.size(); i++)
	{
        enemyAvrg = enemyNear[i]->getAveragePos();
		if (enemyNear[i]->getNShipsAlive() > 0)
		{
			sumHP = 0;
			for (unsigned int j = 0; j < enemyNear[i]->nShips(); ++j)
			{
				sumHP += enemyNear[i]->getShip(j)->getHP();
			}

			int maxHP = enemyNear[i]->getUnitInfo()->squadSize * enemyNear[i]->getUnitInfo()->stats.maxHP;

			// Considerar apenas o inteiro
			int percent = (sumHP / maxHP) * 100;
			if (percent < minHP)
			{
				minHP = percent;
				wekeastUnit = enemyNear[i];
				squad->setTarget( i );
			}
		}

	}

	//ataca as naves do esquadrao de maneira aleatoria
	if (wekeastUnit)
	{
		for (unsigned int i = 0; i < squad->nShips(); ++i)
		{
			Ship *iShip = squad->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % wekeastUnit->nShips();
					if (wekeastUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, wekeastUnit->getShip(s), squad->getUnitInfo(), wekeastUnit->getUnitInfo()));
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
AttackCollab::AttackCollab(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_COLLAB)
{
}

int AttackCollab::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits, const vector<Unit*>& alliedUnits)
{
	squad->setTarget(-1);

    // TODO: Algumas unidades chegam aqui com aliados invalidos! arrumar!
	if (info.allyUnitID >= alliedUnits.size())
        return 0;

	Unit *allyUnit = alliedUnits[info.allyUnitID];
	if (allyUnit->getTarget() == -1)
        return 0;

    int target = allyUnit->getTarget();

    if (target >= enemyUnits.size() || target < 0)
        return 0;

	squad->setTarget(target);

	Unit *enemyUnit = enemyUnits[target];
	if (enemyUnit->getNShipsAlive() == 0){
		return 0;
	}

	/// TODO: Adicionar restricao de distancia?

    float dist = squad->getAveragePos().distance(enemyUnit->getAveragePos());
    if (dist < squad->getUnitInfo()->stats.range)
    {
        for (unsigned int i = 0; i < squad->nShips(); ++i)
		{
			Ship *iShip = squad->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
				while (1)
				{
					int s = rand() % enemyUnit->nShips();
					if (enemyUnit->getShip(s)->isAlive())
					{
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
						newActions.push_back(new AttackAction(iShip, enemyUnit->getShip(s), squad->getUnitInfo(), enemyUnit->getUnitInfo()));
						break;
					}
				}
			}
		}
    }
    else
    {
        Coordinates coord = squad->getAveragePos();
        float direction = atan2(enemyUnit->getAveragePos().y - coord.y, enemyUnit->getAveragePos().x - coord.x);

        for (unsigned int i = 0; i < squad->nShips(); ++i)
		{
		    Ship *iShip = squad->getShip(i);

            if (iShip->isAlive())
            {
                coord = iShip->getPosition();
                coord.x += cos(direction)*SPACIAL_UNIT;
                coord.y += sin(direction)*SPACIAL_UNIT;

                newActions.push_back(new MoveAction( iShip, coord ) );
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
DefenseCollab::DefenseCollab(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_DEFENSE_COLAB)
{

}

int DefenseCollab::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits, const vector<Unit*>& alliedUnits)
{
	int Ret = 0;
	Unit *enemyUnit = NULL;

	for (unsigned int j = 0; j < enemyUnits.size(); j++)
	{
		//interrompe caso tenha encontrado unimigo a quem atacar
		if (enemyUnits[j]->getTarget() == info.allyUnitID)
		{
			enemyUnit = enemyUnits[j];
			squad->setTarget(j);
			break;
		}
	}

	if (enemyUnit == NULL || enemyUnit->getNShipsAlive() == 0)
		return 0;

	for (unsigned int i = 0; i < squad->nShips(); i++)
	{
		Ship *iShip = squad->getShip(i);
		if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
		{
			Coordinates shipPos = iShip->getPosition();
			Ship *nearestShip = NULL;
			float dist_ = 99999;

			for (unsigned int u = 0; u < enemyUnit->nShips(); ++u)
			{
				Ship *eShip = enemyUnit->getShip(u);

				if (eShip->isAlive())
				{
					float d = shipPos.distance(enemyUnit->getShip(u)->getPosition());
					if (d < dist_)
					{
						nearestShip = eShip;
						dist_ = d;
					}
				}
			}

            if (nearestShip)
            {
                if (dist_ < iShip->getBaseStats().range)
                {
                    iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
                    newActions.push_back(new AttackAction(iShip, nearestShip, squad->getUnitInfo(), enemyUnit->getUnitInfo()));
                    ++Ret;
                }
                else
                {
                    float direction = atan2(iShip->getY() - nearestShip->getPosition().y, iShip->getX() - nearestShip->getPosition().x);
                    iShip->move(-iShip->getBaseStats().speed * cos(direction), -iShip->getBaseStats().speed * sin(direction));
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

Kamikase::Kamikase(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_KAMIKASE)
{

}

int Kamikase::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits, const vector<Unit*>& alliedUnits)
{
	if (enemyUnits.size() == 0)
		return 0;

	int Ret = 0;
	float minDist = 99999;

	Coordinates myPos = squad->getAveragePos();
	if (squad->getNShipsAlive() == 0)
		return 0;

	Unit *nearestUnit = NULL;
	for (unsigned int i = 0; i < enemyUnits.size(); ++i)
	{
		Coordinates enemyAvrg = enemyUnits[i]->getAveragePos();
		if (enemyUnits[i]->getNShipsAlive() > 0)
		{
			float dist = myPos.distance(enemyAvrg);
			if (dist < minDist)
			{
				minDist = dist;
				nearestUnit = enemyUnits[i];
				squad->setTarget(i);
			}
		}
	}

	if (nearestUnit && minDist < squad->getSquadBaseStats().range)
	{
		for (unsigned int i = 0; i < squad->nShips(); ++i)
		{
			Ship *iShip = squad->getShip(i);

			if (iShip->isAlive() && iShip->getStats().curKamikazeCD == 0)
			{
				while (1)
				{
					int s = rand() % nearestUnit->nShips();
					if (nearestUnit->getShip(s)->isAlive())
					{
						iShip->getStats().curKamikazeCD = FRAMES_PER_SECOND * 3;
						iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD * 3;
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

Retreat::Retreat(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_RETREAT)
{
}

int Retreat::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits, const vector<Unit*>& alliedUnits)
{
	Coordinates myPos, motherAvrg, enemyAvrg;

	Unit *motherShipUnit = NULL;
	for (unsigned int i = 0; i < alliedUnits.size(); ++i)
	{
		if (alliedUnits[i]->getType() == 0)
		{
			motherShipUnit = alliedUnits[i];
			break;
		}
	}

	motherAvrg = motherShipUnit->getAveragePos();
	myPos = squad->getAveragePos();

	if (myPos.distance(motherAvrg) > squad->getSquadBaseStats().range)
	{
		for (unsigned int i = 0; i < squad->nShips(); i++)
		{
			Ship *iShip = squad->getShip(i);
			if (iShip->isAlive())
			{
				motherAvrg = motherShipUnit->getAveragePos();
				motherAvrg.x += rand() % 300 - rand() % 300;
				motherAvrg.y += rand() % 300 - rand() % 300;

				for (unsigned int i = 0; i < squad->nShips(); ++i)
				{
					Coordinates rndCoord((rand() % 128) - (rand() % 128), (rand() % 128) - (rand() % 128));

					newActions.push_back(new MoveAction(squad->getShip(i), motherAvrg + rndCoord));
				}
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
	str.append(std::to_string(info.allyUnitID));

	return str;
}

// ---

MoveRandomly::MoveRandomly(TacticInfo Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_MOVE_RANDOM)
{
	cooldown = 0;
}

int MoveRandomly::validateTactic(list<Action*> &newActions, Unit* squad, const vector<Unit*>& enemyUnits, const vector<Unit*>& alliedUnits)
{
	if (cooldown > 0)
	{
		--cooldown;
		return 0;
	}

    // TODO: Tamanho da tela de combate? Considerar lado da tela
	Coordinates coordBase = squad->getAveragePos();
	coordBase.x += rand() % 300 - rand() % 300;
	coordBase.y += rand() % 300 - rand() % 300;

	for (unsigned int i = 0; i < squad->nShips(); ++i)
	{
		Coordinates rndCoord((rand() % 128) - (rand() % 128), (rand() % 128) - (rand() % 128));

		newActions.push_back(new MoveAction(squad->getShip(i), coordBase + rndCoord));
	}

	cooldown = (rand() % 120) + 60;

	return 1;
}
