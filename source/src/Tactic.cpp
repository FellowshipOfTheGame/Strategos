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

#include "CombatData.h"

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

int AttackNearestEnemy::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	if (tvd.enemyUnits.size() == 0 || tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	int Ret = 0;

    // Encontrar unidade mais proxima
    float minDist;
	Unit *nearestUnit = tvd.combatData.getNearestUnit(tvd.validatingUnit, tvd.enemyUnits, minDist);
	if (nearestUnit == nullptr) return 0;

    tvd.validatingUnit->setTarget( nearestUnit );

	if (minDist < tvd.validatingUnit->getSquadBaseStats().range2)
	{
		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
                int s = tvd.combatData.randomengine.nextInt() % nearestUnit->nShips();
				while (!nearestUnit->getShip(s)->isAlive())
                    s = tvd.combatData.randomengine.nextInt() % nearestUnit->nShips();

                iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
                newActions.push_back(new AttackAction(iShip, nearestUnit->getShip(s)));
                ++Ret;
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

int AttackWeakestEnemy::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	if ( tvd.enemyUnits.empty() )
		return 0;

	tvd.validatingUnit->setTarget( nullptr );

	int Ret = 0;
	float minHP = 100;

	if (tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	Unit *wekeastUnit = nullptr;

	// Encontrar unidade mais fraca
	for (unsigned int i = 0; i < tvd.enemyUnits.size(); i++)
	{
	    // Evitar selecionar uma unidade sem naves vivas
		if (tvd.enemyUnits[i]->getNShipsAlive() == 0) continue;

        float dist = tvd.combatData.getUnitDistance(tvd.validatingUnit, tvd.enemyUnits[i]);
        if (dist < tvd.validatingUnit->getSquadBaseStats().range2)
        {
            // Verificar HP desta unidade
            float sumHP = tvd.enemyUnits[i]->getHPsum();
            float maxHP = tvd.enemyUnits[i]->getUnitInfo()->squadSize * tvd.enemyUnits[i]->getUnitInfo()->stats.maxHP;

            // Considerar apenas o inteiro
            int percent = (sumHP/(float)maxHP) * 100;
            if (percent < minHP)
            {
                minHP = percent;
                wekeastUnit = tvd.enemyUnits[i];
            }
        }
	}

    if (wekeastUnit == nullptr) return 0;
    tvd.validatingUnit->setTarget( wekeastUnit );

	// Atacar naves da unidade aleatoriamente
    for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
    {
        Ship *iShip = tvd.validatingUnit->getShip(i);

        if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
        {
            int s = tvd.combatData.randomengine.nextInt() % wekeastUnit->nShips();
            while ( !wekeastUnit->getShip(s)->isAlive() ){
                s = tvd.combatData.randomengine.nextInt() % wekeastUnit->nShips();
            }

            iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
            newActions.push_back(new AttackAction(iShip, wekeastUnit->getShip(s)));
            ++Ret;
        }
    }

	return Ret;
}

// ---
AttackCollab::AttackCollab(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_ATTACK_COLLAB)
{
}

int AttackCollab::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	tvd.validatingUnit->setTarget( nullptr );

	const Unit *allyUnit = info.allyUnit;
	if (allyUnit->getTarget() == nullptr)
        return 0;

    Unit* enemyUnit = allyUnit->getTarget();
	if (enemyUnit->getNShipsAlive() == 0){
		return 0;
	}

	tvd.validatingUnit->setTarget( enemyUnit );

    float dist = tvd.combatData.getUnitDistance(tvd.validatingUnit, enemyUnit);
//    float dist = tvd.validatingUnit->getAveragePos().distance(enemyUnit->getAveragePos());

    // Atacar se estiver ao alcance
    if (dist < tvd.validatingUnit->getUnitInfo()->stats.range2)
    {
        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().currentAtkCD == 0)
			{
                int s = tvd.combatData.randomengine.nextInt() % enemyUnit->nShips();
				while ( !enemyUnit->getShip(s)->isAlive() ){
                    s = tvd.combatData.randomengine.nextInt() % enemyUnit->nShips();
				}

				iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
                newActions.push_back(new AttackAction(iShip, enemyUnit->getShip(s)));
			}
		}
    }
    else // Se estiver longe, Mover uma unidade espacial em direcao ao alvo
    {
        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
		    Ship *iShip = tvd.validatingUnit->getShip(i);

            if (iShip->isAlive() && iShip->getStats().isMoving != move_action)
            {
                double direction = atan2( enemyUnit->getAveragePos().y-iShip->getY(), enemyUnit->getAveragePos().x-iShip->getX() );

                Coordinates dest = iShip->getPosition();
                dest.x += SPACIAL_UNIT*cos(direction);
                dest.y += SPACIAL_UNIT*sin(direction);

                newActions.push_back( new MoveAction(iShip, dest, false) );
            }
		}
    }

	return 1;
}

std::string AttackCollab::printTactic()
{
	std::string str = Tactic::printTactic();

	str.append(" ");
	str.append(std::to_string(info.allyUnit->getID()));

	return str;
}

// ---
DefenseCollab::DefenseCollab(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_DEFENSE_COLLAB)
{

}

int DefenseCollab::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	int Ret = 0;
	Unit *enemyUnit = nullptr;

	for (unsigned int j = 0; j < tvd.enemyUnits.size(); j++)
	{
		// Interrompe caso tenha encontrado um inimigo para atacar
		if (tvd.enemyUnits[j]->getTarget() == info.allyUnit)
		{
			enemyUnit = tvd.enemyUnits[j];
			break;
		}
	}

	if (enemyUnit == nullptr || enemyUnit->getNShipsAlive() == 0)
		return 0;

    tvd.validatingUnit->setTarget( enemyUnit );

    // Para cada ship da unidade
	for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); i++)
	{
		Ship *iShip = tvd.validatingUnit->getShip(i);
		// Encontrar alvo
		if ( iShip->isAlive()
        && ( iShip->getStats().currentAtkCD == 0 || iShip->getStats().isMoving != move_action ) )
		{
			const Coordinates& shipPos = iShip->getPosition();
			Ship *nearestShip = nullptr;
			float dist_ = 999999.0f;

            // Verificar ships inimigas
			for (unsigned int u = 0; u < enemyUnit->nShips(); ++u)
			{
				Ship *eShip = enemyUnit->getShip(u);
				if (!eShip->isAlive()) continue;

                float d = shipPos.distance2(eShip->getPosition());
                if (d < dist_)
                {
                    nearestShip = eShip;
                    dist_ = d;
                }
			}

            if (nearestShip == nullptr) continue;

            if (iShip->getStats().currentAtkCD == 0 && dist_ < iShip->getBaseStats().range2)
            {
                iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD;
                newActions.push_back(new AttackAction(iShip, nearestShip));
                ++Ret;
            }
            else if (iShip->getStats().isMoving != move_action) // Mover uma unidade espacial em direcao ao alvo
            {
                double direction = atan2( nearestShip->getY()-iShip->getY(), nearestShip->getX()-iShip->getX() );

                Coordinates dest = iShip->getPosition();
                dest.x += SPACIAL_UNIT*cos(direction);
                dest.y += SPACIAL_UNIT*sin(direction);

                newActions.push_back( new MoveAction(iShip, dest, false) );
                ++Ret;
            }
		}
	}

	return Ret;
}

std::string DefenseCollab::printTactic()
{
	std::string str;

	str = Tactic::printTactic();

	str.append(" ");
	str.append(std::to_string(info.allyUnit->getID()));

	return str;
}

// ---

Kamikase::Kamikase(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_KAMIKASE)
{
}

int Kamikase::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	if (tvd.enemyUnits.size() == 0)
		return 0;
	if (tvd.validatingUnit->getNShipsAlive() == 0)
		return 0;

	int Ret = 0;

    // Encontrar unidade mais proxima
    float minDist;
	Unit *nearestUnit = tvd.combatData.getNearestUnit(tvd.validatingUnit, tvd.enemyUnits, minDist);
	if (nearestUnit == nullptr) return 0;

    tvd.validatingUnit->setTarget( nearestUnit );

	if (minDist < tvd.validatingUnit->getSquadBaseStats().range2)
	{
		for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
		{
			Ship *iShip = tvd.validatingUnit->getShip(i);

			if (iShip->isAlive() && iShip->getStats().isKamikasing == false)
			{
                int s = tvd.combatData.randomengine.nextInt()%nearestUnit->nShips();
				while (!nearestUnit->getShip(s)->isAlive()){
                    s = tvd.combatData.randomengine.nextInt()%nearestUnit->nShips();
				}

                iShip->getStats().isKamikasing = true;
                iShip->getStats().currentAtkCD = iShip->getBaseStats().maxAtkCD * 99;
                newActions.push_back(new KamikazeAction(iShip, nearestUnit->getShip(s), nullptr));
                ++Ret;
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

int Retreat::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	const Unit *allied = info.allyUnit;

	if (allied->getNShipsAlive() == 0)
        return 0;

    Coordinates motherAvrg = allied->getAveragePos();
	if (tvd.validatingUnit->getAveragePos().distance2(motherAvrg) > tvd.validatingUnit->getSquadBaseStats().range2)
	{
        motherAvrg.x += (tvd.combatData.randomengine.nextInt()%300) - 150;
        motherAvrg.y += (tvd.combatData.randomengine.nextInt()%300) - 150;

        // Se alguma nave estiver se movendo, esperar
        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
        {
            if (!tvd.validatingUnit->getShip(i)->isAlive()) continue;

            if (tvd.validatingUnit->getShip(i)->getStats().isMoving == move_action)
                return 0;
        }

        // Manda todas as naves recuarem
        for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
        {
            if (!tvd.validatingUnit->getShip(i)->isAlive()) continue;

            Coordinates rndCoord((tvd.combatData.randomengine.nextInt()%64) - 32,
                                 (tvd.combatData.randomengine.nextInt()%64) - 32 );
            newActions.push_back(new MoveAction(tvd.validatingUnit->getShip(i), motherAvrg + rndCoord, false));
        }
	}

	return 1;
}

std::string Retreat::printTactic()
{
	std::string str;

	str = Tactic::printTactic();

	str.append(" ");
    str.append(std::to_string(info.allyUnit->getID()));

	return str;
}

// ---

MoveRandomly::MoveRandomly(const TacticInfo& Info, const TacticTrigger& trigger) :
		Tactic(Info, trigger, TACTIC_MOVE_RANDOM)
{
	cooldown = 0;
}

int MoveRandomly::validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd)
{
	if (cooldown > 0)
	{
		--cooldown;
		return 0;
	}

    // Se alguma nave estiver se movendo, esperar
	for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
	{
	    if (!tvd.validatingUnit->getShip(i)->isAlive()) continue;

	    if (tvd.validatingUnit->getShip(i)->getStats().isMoving == move_action)
            return 0;
	}

	Coordinates coordBase = tvd.validatingUnit->getAveragePos();
	coordBase.x += (tvd.combatData.randomengine.nextInt()%300) - 150;
	coordBase.y += (tvd.combatData.randomengine.nextInt()%300) - 150;

	for (unsigned int i = 0; i < tvd.validatingUnit->nShips(); ++i)
	{
	    if (!tvd.validatingUnit->getShip(i)->isAlive()) continue;

        Coordinates rndCoord((tvd.combatData.randomengine.nextInt()%64) - 32,
                             (tvd.combatData.randomengine.nextInt()%64) - 32 );
        newActions.push_back(new MoveAction(tvd.validatingUnit->getShip(i), coordBase + rndCoord, false));
	}

	cooldown = (tvd.combatData.randomengine.nextUInt() % 90) + 30;

	return 1;
}
