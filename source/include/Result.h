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

enum GRAPHS{
    DMG_DEALT,
    DMG_TAKEN,
    KILLS,
    DEATHS,
};

class Graph
{
    public:
        void drawGraph(SDL_Renderer *renderer)
        {
            const int offX = 62;
            const int offY = 600;
            const int yscale = 500;

            for (std::map<int, double>::const_iterator it = data.begin(); it != data.end(); )
            {
                auto next = it++;

                if (next != data.end())
                    SDL_RenderDrawLine( renderer, offX + it->first, offY - it->second*yscale, offX + next->first, offY - next->second*yscale );
                else
                    break;
            }
        }

        void addDot(int x, double fx){
            data[x] = fx;
        }

    private:
        std::map<int, double> data;
};

class Result : public StateMachine
{
    private:
        Image *imgBackground;
        Button *btn_Next;

        Label *alive,*dead;
        CombatLog *original_log1, *original_log2;

        Graph army0[4];
        Graph army1[4];

        void normalizeRounds(const CombatRound* l1, const CombatRound* l2);

    public:
        Result(STATE previous);
        ~Result();

        void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
        void Logic();
        void Render();
        void Clean();
};
#endif /* RESULT_H_ */
