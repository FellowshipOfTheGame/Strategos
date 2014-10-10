/*
 *  Strategos - CREATE_ARMY.h
 *
 *  Este eh uma visualizacao do STATE CREATE_ARMY.
 *	O CREATE_ARMY sera a primeira tela a ser exibida e eh o retorno
 *  para quando os outros estados acabarem.
 *
 *	Last update: 06/07/2012
 */

#ifndef _UNIT_SETUP_H_
#define _UNIT_SETUP_H_

#include "StateMachine.h"
#include "GuiElements.h"
#include "GuiTactics.h"
#include "Dictionary.h"

class Unit_Setup : public StateMachine
{
    public:
        Unit_Setup(STATE previous);
        ~Unit_Setup();

        void onInputEvent(GuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
        void Logic();
        void Render();
        void Clean();

    private:
        const Image *imgBackground;
        const Image *squad_selec;
        ImageBox *blueprint;
        Button *btn_Back;
        Button *btn_Next;
        Button *btn_Del;
        Button *btn_Move;
        Unit *squad_focus;

        TacticList *list;

        Label *lbl_Geral;

        SDL_Texture* renderCombat;

        ImageBox *bx1,*bx2,*bx3,*bx4;
        StatusBox *st_box;

        Dictionary *dct;
        std::vector<Label*> squad_number;

        bool put_squad, move_squad;
        int squad_type;

        RandomEngine rnd;
};

#endif
