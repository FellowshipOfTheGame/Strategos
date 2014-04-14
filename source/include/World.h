/*
 *  Strategos - World.h
 *
 *  Classe usada para fazer a simulacao de combate entre duas 'Army'
 *
 */


#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>

#include "Camera.h"
#include "Army.h"
#include "CombatData.h"
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

        CombatData combatData;
        TacticValidationData tvdForArmy1;
        TacticValidationData tvdForArmy2;

        int totalSteps;

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

        // Desenha o combate no renderer principal
        void render();

};

#endif
