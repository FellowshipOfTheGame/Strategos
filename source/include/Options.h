/*
 *  Strategos - Options.h
 *
 *  Este eh uma visualizacao do STATE Options.
 *	O Options sera a tela que permitira configurar
 *  Som(on/off), Volume, Tela_Fullscreen(on/off)
 *
 *	Last update: 06/07/2012
 */

#ifndef _Options_H_
#define _Options_H_

#include "StateMachine.h"

class Options : public StateMachine
{
private:

public:
	Options(STATE previous);
	~Options();

    void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
	void Logic();
	void Render();
	void Clean();
};

#endif
