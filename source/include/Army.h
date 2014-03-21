/*
 *  Strategos - Army.h
 *
 *  Esta classe representa um exercito em jogo, podendo ser do computador
 *	ou do jogador.
 *	O exercito contem um vetor de unidades que sao os aglomerados de naves
 *  utilizadas em combate. (@see Unit.h)
 *
 *	Last update: 06/07/2012
 */


#ifndef _ARMY_H_
#define _ARMY_H_

#include <string>

#include "Unit.h"
#include "Tactic.h"
#include "Dictionary.h"

#define COMBAT_AREA_WIDTH 800
#define COMBAT_AREA_HEIGHT 700

#define N_UNIT_TYPE             4

using namespace std;

class Army
{
private:
	std::string name;
	const Dictionary *dictionary;

    Unit* motherUnit; // Unidade mae
	std::vector<Unit*> units; // Todas as unidades da army, incluindo unidade Mae
	int totalShips;

	Image *shipsGFX;

public:
	Army(std::string armyName, const Dictionary* armyDictionary);
	~Army();

    // Altera a posicao base de todo o Army
    // Todas as naves movem relativamente a pos
    void setBasePos(Coordinates pos);

    // Percorre Tactics e gera acoes para as naves
    int update();
    void updateActions();

    void restore();

    // Adiciona um Unit para o Army
    void addUnit(Unit *unit);
	Unit* createUnit(int id, int unitType, Coordinates position);
	Unit* removeUnit(int i);

	const Dictionary *getDictionary() const;
	void setDictionary(Dictionary* armyDictionary);
	void setArmyName(std::string armyName);

	void setReflectBasePositions(int startWidth);

	// Gets
	Unit* getMotherUnit();
	const std::string& getName() const;
	int getTotalShips() const;
	const std::vector<Unit*>& getUnits() const;
    unsigned int nUnits() const;
    vector<Unit*>* getUnitsReference();

    /// Pegar unidade de acordo com o ID dela
    Unit* getUnitByID(unsigned int id) const;

    /// Pegar unidade de acordo com a posicao dela no vetor
    Unit* getUnitAtIndex(unsigned int id) const;

	void printUnits() const;

    void render();

    static Army* loadArmy(std::string armyname);
    static void saveArmy(const Army *army, const string pth="");
};

#endif
