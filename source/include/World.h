/*
 *  Strategos - World.h
 *
 *  Esta classe eh um dos 'Controller' (@see MVC) do nosso software.
 *	Ela vai comunicar os exercitos (@see Army.h) com a simulacao
 *	propriamente dita.
 *
 *	Last update: 06/07/2012
 */


#ifndef _WORLD_H_
#define _WORLD_H_

#include "Camera.h"
#include "Army.h"
#include <vector>
#include "CombatLog.h"

#define _SIM_CONTINUE_  0
#define _SIM_ARMY0_WIN_ 1
#define _SIM_ARMY1_WIN_ 2
#define _SIM_DRAW_      3

using namespace std;

class World
{
    private:
        vector<Army*> armies;
        void makeLog(Army *army, CombatLog *log);

    public:
        World(Army *army1, Army *army2);
        ~World();

        // Calcula valores necessarios para as acoes: Distancia,...
        void calcSimulationData();

        // Gera vetor de Actions para cada Army
        void calcActions();

        //Simula um "Frame" depois Atualizamos renderizacao/input e etc.
        int simulateStep();


        Army *loadArmy(string path);
        void printLoadedArmy();

        Army* getArmy(int id);

        //Unit *getUnit(int armyID, int unitID);
        //Ship *getShip(int armyID, int shipID);

        void render(Camera *camera);

};

#endif
