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

class Result : public StateMachine
{
private:
    Image *imgBackground;
    Button *btn_Next;

    Label *alive,*dead;
    CombatLog *original_log1, *original_log2;

    CombatRound normalized_generalLog1, normalized_generalLog2;

    void normalizeRounds(const CombatRound* l1, const CombatRound* l2, const CombatRound& out1, const CombatRound& out2);

public:
    Result(STATE previous);
    ~Result();

    void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
    void Logic();
    void Render();
    void Clean();
};
#endif /* RESULT_H_ */
