/*
 *  Strategos - Army.cpp
 *
 */
#include <stdio.h>

#include "Global.h"
#include "Trigger.h"
#include "Army.h"

using namespace std;

Army* Army::clone( const Army* army )
{
    Army *clone = new Army(army->name, army->dictionary);
    clone->fitness = army->getFitness();

    for (int i = 0; i < army->units.size(); ++i)
    {
        clone->addUnit( new Unit( army->units[i] ) );
    }

    return clone;
}

Army::Army(const string& armyName, const Dictionary *armyDictionary) :
		name(armyName), dictionary(armyDictionary), motherUnit(0), fitness(0)
{
	totalShips = 0;
	isPlayer = 0;
	fitness =0;
}

Army::~Army()
{
	for (unsigned int i = 0; i < units.size(); ++i){
		delete units[i];
	}
}

void Army::Lock()
{
    army_mutex.lock();
}

void Army::Unlock()
{
    army_mutex.unlock();
}

Army* Army::loadArmy(string armyname)
{
	Army *loadedArmy = nullptr;
	string path;
	path.append(SAVE_PATH);
	path.append(armyname);
	path.append(SAVE_EXT);
	fstream file;
	Dictionary *workingDict;
	string tag, name, dictName;
	int unitType, id = 0, isPlayer;
	float fitness;

	Coordinates position;

	printf("Loading Army: %s\n", path.c_str());

	file.open(path.c_str(), fstream::in);
	if (!file.is_open())
	{
		printf("error on loading Army!\n");
		return nullptr;
	}

	while (!file.eof())
	{
		file >> tag;

		if (tag.compare("name:") == 0)
		{
			file >> name;
			printf("\tName: %s\n", name.c_str());
		}
		else if (tag.compare("fitness:") == 0)
		{
			file >> fitness;
			printf("\tFitness: %lf\n", fitness);
		}
		else if (tag.compare("player:") == 0)
		{
			file >> isPlayer;
			printf("\tPlayer: %lf\n", isPlayer);
		}
		else if (tag.compare("dictionary:") == 0)
		{
			file >> dictName;
			workingDict = Game::getGlobalGame()->getDictionary(dictName);
			if (!workingDict)
			{
				printf("Erro ao carregar Army!\n");
				printf("Este tipo de Army nao existe: \"%s\"!\n", dictName.c_str());

				// TODO: verificar memory leaks
				delete loadedArmy;

				return nullptr;
			}

			loadedArmy = new Army(name, workingDict);
			if (isPlayer)
            {
                loadedArmy->setIsPlayer();
            }
			loadedArmy->setFitness(fitness);
			printf("\tDict: %s\n", dictName.c_str());
		}
		else if (tag.compare("squadKind:") == 0)
		{
			file >> unitType;

			// Ler posicao
			file >> tag;
			file >> position.x;
			file >> position.y;

			Unit *unit = loadedArmy->createUnit(id, unitType, position);
			++id;
			if (!unit)
			{
				printf("Erro ao carregar Army!\n");
				printf("Este tipo de Squad nao existe: %d!\n", unitType);

				// TODO: verificar memory leaks
				delete loadedArmy;

				return nullptr;
			}
			//unit->setShipImages(workingDict->getShipImage());

            // RULE: [trigger(tipo, valor, operador)][logic][trigger(tipo, valor, operador)][ruleType][extra]
			while (tag != "END_SQUAD")
			{
				file >> tag;
				if (tag != "rule:")
					continue;

				// Ler Triggers
				Trigger *trig1=0, *trig2=0;
				int trigger, value, operation, logic;

				// LER PRIMEIRO TRIGGER
				file >> trigger;
				file >> value;
                file >> operation;
//                printf("%d%d.%d", trigger, value, operation);
				switch (trigger)
				{
					case TRIGGER_ALWAYS:
						trig1 = new Trigger_Always();
                    break;

					case TRIGGER_LIFE:
						trig1 = new Trigger_Life(value, operation);
                    break;

                    default:
                        printf("ERROR: Unkown Trigger: %d\n", trigger);
                        delete loadedArmy;

                        return nullptr;
                    break;
				}

				// Ler Operador Logico
				file >> logic;
//				printf("%d", logic);

				// LER SEGUNDO TRIGGER
				file >> trigger;
				file >> value;
                file >> operation;
//                printf("%d%d.%d", trigger, value, operation);
				switch (trigger)
				{
					case TRIGGER_ALWAYS:
						trig2 = new Trigger_Always;
                    break;

					case TRIGGER_LIFE:
						trig2 = new Trigger_Life(value, operation);
                    break;

                    default:
                        printf("ERROR: Unkown Trigger: %d\n", trigger);
                        delete loadedArmy;

                        return nullptr;
                    break;
				}

				TacticTrigger tacticTrigger(trig1, trig2, logic);

				// Ler valores de tatica
				Tactic *tatica = nullptr;
				int ruleID;
				file >> ruleID;
//				printf("%d", ruleID);

				TacticInfo info(0);

				switch (ruleID)
				{
					case TACTIC_ATTACK_NEAREST_ENEMY:
						tatica = new AttackNearestEnemy(info, tacticTrigger);
						break;

					case TACTIC_ATTACK_WEAKEST_ENEMY:
						tatica = new AttackWeakestEnemy(info, tacticTrigger);
						break;

					case TACTIC_ATTACK_COLLAB:
						file >> info.allyUnitID;
						tatica = new AttackCollab(info, tacticTrigger);
						break;

					case TACTIC_DEFENSE_COLAB:
						file >> info.allyUnitID;
						tatica = new DefenseCollab(info, tacticTrigger);
						break;

					case TACTIC_KAMIKASE:
						tatica = new Kamikase(info, tacticTrigger);
						break;

					case TACTIC_RETREAT:
						file >> info.allyUnitID;
						tatica = new Retreat(info, tacticTrigger);
						break;

					case TACTIC_MOVE_RANDOM:
						tatica = new MoveRandomly(info, tacticTrigger);
						break;
				}

				unit->addTactic(tatica);
//				printf(" - ");
			}
		}
	}
	printf("Army successfully loaded\n");
	return loadedArmy;
}

void Army::saveArmy(const Army *army, const string pth)
{
	string path;
	fstream file;

	path.append(SAVE_PATH);
	path.append(pth.c_str());
	path.append(army->getName());
	path.append(SAVE_EXT);
	printf ("saving %s\n",path.c_str());
	file.open(path.c_str(), ios::out);

	const std::vector<Unit*>& units = army->getUnits();

	file << "name: " << army->getName() << endl;

	file << "fitness: " << army->getFitness() << endl;

	file << "player: " << army->getIsPlayer() << endl;

	file << "dictionary: " << army->getDictionary()->title << endl << endl;

	for (unsigned int i = 0; i < units.size(); i++)
	{
		file << "squadKind: " << units[i]->getType() << endl;
		file << "position: " << units[i]->getBluePrintX() << " " << units[i]->getBluePrintY() << endl;

		for (unsigned int j = 0; j < units[i]->getTacticSize(); j++)
		{
			file << "rule: ";
			file << units[i]->getTacticAt(j)->printTactic() << endl;
		}

		file << "END_SQUAD" << endl << endl;
	}

	file.close();
	printf ("Army's ready\n");
}

// Sets

void Army::setDictionary(Dictionary* armyDictionary)
{
	this->dictionary = armyDictionary;
}

void Army::setArmyName(const std::string& armyName)
{
	this->name = armyName;
}

void Army::addUnit(Unit *unit)
{
    totalShips += unit->getUnitInfo()->squadSize;

    if (unit->getType() == 0)
    {
        if (motherUnit != 0)
        {
            printf("ANOTHER MOTHER ADDED!\n");
            exit(-1);
        }
		motherUnit = unit;
	}

    units.push_back(unit);
}

Unit *Army::createUnit(int id, int unitType, Coordinates position)
{
	const DictKey *unitInfo = dictionary->getInfoFor(unitType);
	Unit *unit = new Unit(id, unitInfo, position);

	addUnit(unit);

	return unit;
}

Unit* Army::removeUnit(unsigned int i)
{
	if (i > units.size())
		return nullptr;

    auto it = (units.begin() + i);
    Unit *removed = *it;

	totalShips -= (*it)->nShips();

	units.erase(it);
	vector<Unit*>::iterator iter = units.begin();
	i = 0;
	while (iter != units.end())
	{
		(*iter)->setID(i);
		iter++;
		i++;
	}

	return removed;
}

// Gets

Unit* Army::getMotherUnit()
{
	//return motherUnit;
	return units[0];
}

int Army::getTotalShips() const
{
	return totalShips;
}

const Dictionary *Army::getDictionary() const
{
	return dictionary;
}

Unit* Army::getUnitByID(unsigned int id) const
{
	vector<Unit*>::const_iterator iter = units.begin();

	while (iter != units.end())
	{
		if ((*iter)->getID() == id)
			return (*iter);

		iter++;
	}

	return nullptr;
}

Unit* Army::getUnitAtIndex(unsigned int id) const
{
	if (id < units.size())
		return units[id];
	else
		return nullptr;
}

const vector<Unit*>& Army::getUnits() const
{
	return units;
}

vector<Unit*>* Army::getUnitsReference()
{
	return &units;
}

unsigned int Army::nUnits() const
{
	return units.size();
}

const std::string& Army::getName() const
{
	return name;
}

void Army::restore(int asTeam)
{
    int offX = 0;
    int offY = 0;

    if (asTeam == 1)
    {
        offX = TEAM_2_POSX;
        offY = TEAM_2_POSY;

        for (unsigned int i = 0; i < units.size(); i++){
            const int x = TEAM_AREA_WIDTH - units[i]->getBluePrintX() + offX;
            const int y = TEAM_AREA_HEIGHT - units[i]->getBluePrintY() + offY;
            units[i]->restoreUnit( 1, Coordinates(x, y) );
        }
    }
    else
    {
        for (unsigned int i = 0; i < units.size(); i++){
            units[i]->restoreUnit( 0 );
        }
    }
}

void Army::updateActions()
{
    for (unsigned int i = 0; i < units.size(); i++)
	{
		units[i]->updateActions();
	}
}

int Army::update()
{
    int t = 0;

    // Se a nave mae estiver morta, o exercito perdeu.
    if (units[0]->getNShipsAlive() == 0)
        return 0;

	for (unsigned int i = 0; i < units.size(); i++)
	{
		t += units[i]->update();
	}

	return t;
}

void Army::printUnits() const
{
	for (unsigned int i = 0; i < units.size(); i++)
	{
		printf("ID: %d\n", units[i]->getID());
		printf("Type: %d\n", units[i]->getType());

		units[i]->printInfo();
		printf("\n");
	}

	printf("-------------------------|-------------------------\n");
}

void Army::render()
{
	for (unsigned int i = 0; i < nUnits(); i++)
	{
		units[i]->render();
	}

//    for (int j = 0; j < 4; ++j)
//    {
//        const DictKey *k = units[j]->getUnitInfo();
//        for (int i = 0; i < 36; ++i){
//          k->shipsGFX[i]->DrawImage(i*37, j*37, Game::getGlobalGame()->getRenderer());
//        }
//    }
}

CombatRound* Army::unifyCombatRound()
{
	printf ("unificando army - begin\n");
	CombatRound* _new;
	_new = new CombatRound();
	 for (unsigned int i = 0; i < units.size(); ++i)
	 {
		 _new = _new->ConcatCombatRound(units[i]->unifyCombatRound());
	 }
	 printf ("unificando army - end\n");
	return _new;
}

void Army::setFitness(float ft)
{
	fitness = ft;
}

float Army::getFitness() const
{
	return fitness;
}
