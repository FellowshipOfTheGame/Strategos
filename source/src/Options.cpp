/*
 *  Strategos - Options.cpp
 *
 */

#include "Options.h"

#include "Global.h"
#include "Game.h"

Options::Options(STATE previous) : StateMachine(previous, OPTIONS, OPTIONS)
{

}

Options::~Options()
{

}


void Options::Logic()
{

}

void Options::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{

}

void Options::Render()
{
	Game::getGlobalGame()->setBackgroundColor(0, 0, 255);
}

void Options::Clean()
{

}
