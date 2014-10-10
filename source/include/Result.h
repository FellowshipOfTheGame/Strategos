/*
 * Result.h
 *
 *  Created on: 21/09/2013
 *      Author: Eldhelion
 */

#ifndef RESULT_H_
#define RESULT_H_

#include "StateMachine.h"
#include "GuiElements.h"
#include "CombatLog.h"

#include "Graph.h"

enum GRAPHS{
    DMG_DEALT,
    DMG_TAKEN,
    KILLS,
    DEATHS,
};

class Result : public StateMachine
{
    public:
        Result(STATE previous);
        ~Result();

        void onInputEvent(GuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button) override;
        void Logic();
        void Render();
        void Clean();

    private:
        const Image *imgBackground;
        Button *btn_Next;
        Label* legendaDD, *legendaDR, *legendaK, *legendaD;

        //Label *alive,*dead;
        CombatLog *original_log1, *original_log2;

        int graphSteps;
        int graphX, graphY, graphW, graphH;

        Graph army0[4];
        Graph army1[4];

        void normalizeRounds(const CombatRound* l1, const CombatRound* l2);

        void reduzir(const CombatRound* original, CombatRound& reduced, int steps, int timeMax);
        int tratar(CombatRound& graph, int total_ships);

};
#endif /* RESULT_H_ */
