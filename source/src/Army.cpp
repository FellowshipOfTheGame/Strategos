/*
 *  Strategos - Army.cpp
 *
 */
#include <stdio.h>

#include "Global.h"
#include "Game.h"

#include "Trigger.h"
#include "Army.h"

using namespace std;

Tactic* Army::loadTactic(std::ifstream& file, const TacticTrigger& tacticTrigger, std::vector<AllyIDTactic>& to_fix_ally)
{
    Tactic *tatica = nullptr;

    int ruleID;
    file >> ruleID;
//				printf("%d", ruleID);

    switch (ruleID)
    {
        case TACTIC_ATTACK_NEAREST_ENEMY:
            tatica = new AttackNearestEnemy(TacticInfo(nullptr), tacticTrigger);
            break;

        case TACTIC_ATTACK_WEAKEST_ENEMY:
            tatica = new AttackWeakestEnemy(TacticInfo(nullptr), tacticTrigger);
            break;

        case TACTIC_ATTACK_COLLAB:{
            int idOnVec;
            file >> idOnVec;

            tatica = new AttackCollab(TacticInfo(nullptr), tacticTrigger);
            to_fix_ally.push_back( AllyIDTactic(tatica, idOnVec) );

            break;
        }

        case TACTIC_DEFENSE_COLLAB:{
            int idOnVec;
            file >> idOnVec;

            tatica = new DefenseCollab(TacticInfo(nullptr), tacticTrigger);
            to_fix_ally.push_back( AllyIDTactic(tatica, idOnVec) );

            break;
        }

        case TACTIC_KAMIKASE:
            tatica = new Kamikase(TacticInfo(nullptr), tacticTrigger);
            break;

        case TACTIC_RETREAT:{
            int idOnVec;
            file >> idOnVec;

            tatica = new Retreat(TacticInfo(nullptr), tacticTrigger);
            to_fix_ally.push_back( AllyIDTactic(tatica, idOnVec) );
            break;
        }

        case TACTIC_MOVE_RANDOM:
            tatica = new MoveRandomly(TacticInfo(nullptr), tacticTrigger);
            break;

        default:
            printf("Unkown tactic: %d\n", ruleID);
            return nullptr;
    }

    return tatica;
}

Trigger* Army::loadTrigger(ifstream& file)
{
    int trigger, value, operation;
    file >> trigger;
    file >> value;
    file >> operation;
//                printf("%d%d.%d", trigger, value, operation);
    switch (trigger)
    {
        case TRIGGER_ALWAYS:
            return new Trigger_Always();

        case TRIGGER_LIFE:
            return new Trigger_Life(value, operation);

        default:
            printf("ERROR: Unkown Trigger: %d\n", trigger);
            return nullptr;
    }

    return nullptr;
}

Army* Army::loadArmy(const string& armyname)
{
	Army *loadedArmy = nullptr;
	string path;
	path.append(SAVE_PATH);
	path.append(armyname);
	path.append(SAVE_EXT);
	Dictionary *workingDict;
	string tag, name, dictName;
	int unitType, isPlayer;
	float fitness;

	Coordinates position;

	printf("Loading Army: %s\n", path.c_str());

    // Abrir arquivo para leitura
	ifstream file( path.c_str() );
	if (!file.is_open())
	{
		printf("error on loading Army!\n");
		return nullptr;
	}

    std::vector<AllyIDTactic> to_fix_ally;

	while (!file.eof())
	{
		file >> tag;

		if (tag.compare("name:") == 0)
		{
			file >> name;
//			printf("\tName: %s\n", name.c_str());
		}
		else if (tag.compare("fitness:") == 0)
		{
			file >> fitness;
//			printf("\tFitness: %lf\n", fitness);
		}
		else if (tag.compare("player:") == 0)
		{
			file >> isPlayer;
			printf("\tPlayer: %d\n", isPlayer);
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
//			printf("\tDict: %s\n", dictName.c_str());
		}
		else if (tag.compare("squadKind:") == 0)
		{
			file >> unitType;

			// Ler posicao
			file >> tag;
			file >> position.x;
			file >> position.y;

			Unit *unit = loadedArmy->createUnit(unitType, position);
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
				int logic;

				// LER PRIMEIRO TRIGGER
				trig1 = loadTrigger(file);
				if (trig1 == nullptr){
                    delete loadedArmy;
                    return nullptr;
                }

				// Ler Operador Logico
				file >> logic;
//				printf("%d", logic);

				// LER SEGUNDO TRIGGER
				trig2 = loadTrigger(file);
				if (trig2 == nullptr){
                    delete loadedArmy;
                    return nullptr;
                }

				// Ler valores de tatica
				Tactic *tatica = loadTactic(file, TacticTrigger(trig1, trig2, logic), to_fix_ally);
				unit->addTactic(tatica);
//				printf(" - ");
			}
		}
	}

	// Fix all tactics to have the Unit* pointer
    for (unsigned int i = 0; i < to_fix_ally.size(); ++i)
    {
        if (to_fix_ally[i].id >= (int) loadedArmy->nUnits())
        {
            printf("ERROR: Unit with invalid tactic ally index: %d, max: %d\n", to_fix_ally[i].id, loadedArmy->nUnits() );
            exit(8);
        }

        to_fix_ally[i].tactic->setInfo( TacticInfo(loadedArmy->getUnitAtIndex(to_fix_ally[i].id) ) );
    }

//	printf("Army successfully loaded\n");
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
//	printf ("saving %s\n",path.c_str());
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

//        printf("NTactics: %d\n", units[i]->getTacticSize());
		for (unsigned int j = 0; j < units[i]->getTacticSize(); j++)
		{
			file << "rule: ";
			file << units[i]->getTacticAt(j)->printTactic() << endl;
		}

		file << "END_SQUAD" << endl << endl;
	}

	file.close();
//	printf ("Army's ready\n");
}

Army* Army::clone( const Army* army )
{
    Army *clone = new Army(army->name, army->dictionary);
    clone->fitness = army->getFitness();

    for (unsigned int i = 0; i < army->units.size(); ++i)
    {
        clone->addUnit( new Unit( army->units[i] ) );
    }

    for (unsigned int i = 0; i < army->units.size(); ++i)
    {
        const vector<Unit*>& units = army->getUnits();
        for (unsigned int j = 0; j < units[i]->getTacticSize(); ++j)
        {
            const Tactic *originalTactic = units[i]->getTacticAt(j);
            if ( originalTactic->getInfo().allyUnit ){
                const int originalID = originalTactic->getInfo().allyUnit->getID();
                clone->getUnitAtIndex(i)->getTacticAt(j)->setInfo( TacticInfo(clone->getUnitAtIndex(originalID)) );
            }
        }
    }

    return clone;
}

Army::Army(const string& armyName, const Dictionary *armyDictionary) :
		name(armyName), dictionary(armyDictionary), motherUnit(0), fitness(0)
{
	totalShips = 0;
	isPlayer = 0;
	fitness = DONT_HAVE_FITNESS;
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

Unit *Army::createUnit(int unitType, Coordinates position)
{
    // Evitar que crie duas naves mae
    if ( unitType == 0 && motherUnit != 0 )
        return nullptr;

	const DictKey *unitInfo = dictionary->getInfoFor(unitType);
	Unit *unit = new Unit(units.size(), unitInfo, position);

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
	for (unsigned int i = 0; i < units.size(); ++i)
    {
        units[i]->setID(i);
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
    return units[id];
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

void Army::restore(int asTeam, CombatLog *log)
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
            units[i]->restoreUnit( 1, Coordinates(x, y), log );
        }
    }
    else
    {
        for (unsigned int i = 0; i < units.size(); i++){
            units[i]->restoreUnit( 0, log );
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

int Army::update(RandomEngine& randE)
{
    if (units.empty()) return 0;

    int t = 0;

    // Se a nave mae estiver morta, o exercito perdeu.
    if (units[0]->getNShipsAlive() == 0)
        return 0;

	for (unsigned int i = 0; i < units.size(); i++)
	{
		t += units[i]->update(randE);
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
//          k->shipsGFX[i]->DrawImage(Game::getGlobalGame()->getRenderer(), i*37, j*37);
//        }
//    }
}


void Army::setFitness(double ft)
{
	fitness = ft;
}

double Army::getFitness() const
{
	return fitness;
}
