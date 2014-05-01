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
#include <mutex>
#include <fstream>

#include "Unit.h"
#include "Tactic.h"
#include "Dictionary.h"

#define TEAM_AREA_WIDTH 800
#define TEAM_AREA_HEIGHT 700

#define COMBAT_ARENA_WIDTH  ((TEAM_AREA_WIDTH*2)+448)
#define COMBAT_ARENA_HEIGHT    TEAM_AREA_HEIGHT

#define TEAM_2_POSX     (COMBAT_ARENA_WIDTH-TEAM_AREA_WIDTH)
#define TEAM_2_POSY     (COMBAT_ARENA_HEIGHT-TEAM_AREA_HEIGHT)

#define N_UNIT_TYPE             4

class Army
{
    private:
        std::string name;
        const Dictionary *dictionary;

        Unit* motherUnit; // Unidade mae
        std::vector<Unit*> units; // Todas as unidades da army, incluindo unidade Mae
        int totalShips;

        std::mutex army_mutex;
        float fitness;
        int isPlayer;

        // Para fazer load
        struct AllyIDTactic{
            AllyIDTactic(Tactic* t, int ID) : tactic(t), id(ID)
            {}

            Tactic* tactic;
            int id;
        };

        static Trigger* loadTrigger(std::ifstream& file);
        static Tactic* loadTactic(std::ifstream& file, const TacticTrigger& tacticTrigger, std::vector<AllyIDTactic>& to_fix_ally);

    public:
        static Army* loadArmy(const std::string& armyname);
        static void saveArmy(const Army *army, const std::string pth="");
        static Army* clone( const Army* army );

        Army(const std::string& armyName, const Dictionary* armyDictionary);
        ~Army();

        void Lock();
        void Unlock();

        // Percorre Tactics e gera acoes para as naves
        int update();
        void updateActions();

        // Resetar Army como o time X
        // Altera o lado das naves
        void restore(int asTeam);

        // Adiciona um Unit para o Army
        void addUnit(Unit *unit);
        Unit* createUnit(int unitType, Coordinates position);
        Unit* removeUnit(unsigned int i);

        const Dictionary *getDictionary() const;
        void setDictionary(Dictionary* armyDictionary);
        void setArmyName(const std::string& armyName);

        // Gets
        Unit* getMotherUnit();
        const std::string& getName() const;
        int getTotalShips() const;
        const std::vector<Unit*>& getUnits() const;
        unsigned int nUnits() const;
        std::vector<Unit*>* getUnitsReference();

        /// Pegar unidade de acordo com o ID dela
        Unit* getUnitByID(unsigned int id) const;

        /// Pegar unidade de acordo com a posicao dela no vetor
        Unit* getUnitAtIndex(unsigned int id) const;

        void printUnits() const;

        void render();

        CombatRound* unifyCombatRound();

        void setFitness(float ft);
        float getFitness() const;

        void setIsPlayer() {
            isPlayer = 1;
        }
         int getIsPlayer() const{
            return isPlayer;
        }
};

#endif
