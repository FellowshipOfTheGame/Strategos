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
            const int xscale = 8;
            const int yscale = 500;

            SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );

            std::map<int, double>::const_iterator itNow = data.begin();
            std::map<int, double>::const_iterator itNext = data.begin();
            while ( ++itNext != data.end() )
            {
                SDL_RenderDrawLine( renderer, offX + itNow->first*xscale, offY - itNow->second*yscale,
                                              offX + itNext->first*xscale, offY - itNext->second*yscale );

                itNow = itNext;
            }
        }

        void addDot(int x, double fx){
            data[x] = fx;
        }

        void setColor( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
        {
            color.r = r;
            color.g = g;
            color.b = b;
            color.a = a;
        }

    private:
        std::map<int, double> data;

        double maxValue;
        SDL_Color color;
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

        void reduzir(const CombatRound* original, CombatRound& reduced, int steps, int timeMax);
        int tratar(CombatRound& graph, int total_ships);

    public:
        Result(STATE previous);
        ~Result();

        void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
        void Logic();
        void Render();
        void Clean();
};
#endif /* RESULT_H_ */
