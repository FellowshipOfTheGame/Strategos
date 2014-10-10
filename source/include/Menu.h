/*
 *  Strategos - Menu.h
 *
 *  Este eh uma visualizacao do STATE Menu.
 *	O Menu sera a primeira tela a ser exibida e eh o retorno
 *  para quando os outros estados acabarem.
 *
 *	Last update: 06/07/2012
 */

#ifndef _MENU_H_
#define _MENU_H_

#include "StateMachine.h"
#include "GuiElements.h"

class Menu : public StateMachine
{
    private:
        const Image *imgBackground;
        const Image *imgStrategos;
        Button *btExit;
        Button *btPlay;
        Button *btCreate;

    public:
        Menu(STATE previous);
        ~Menu();

        void onInputEvent(GuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button) override;
        void Logic();
        void Render();
        void Clean();
};

#endif
