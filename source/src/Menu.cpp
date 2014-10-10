/*
 *  Menu.cpp
 *  Strategos
 *
 *
 */

#include "Menu.h"

#include "Global.h"

#include "Game.h"

Menu::Menu(STATE previous) : StateMachine(previous, MENU, MENU)
{
    Game* globalGame = Game::getGlobalGame();

	Resource *resource = globalGame->getResourceMNGR();
	resource->AddImage("assets/ui.gfx", "bg_menu");
	resource->AddImage("assets/ui.gfx", "bg_strategos_menu");

	resource->AddImage("assets/ui.gfx", "exit-bt");
	resource->AddImage("assets/ui.gfx", "create_army-bt");
	resource->AddImage("assets/ui.gfx", "new_battle-bt");

	resource->AddFont("assets/fonts.gfx", "jostix-14");

	imgBackground = resource->GetImage("bg_menu");
	imgStrategos = resource->GetImage("bg_strategos_menu");

	btPlay = new Button(335, 350, resource->GetImage("new_battle-bt"));
	addGuiElement(btPlay);

	btCreate = new Button(332, 420, resource->GetImage("create_army-bt"));
	addGuiElement(btCreate);

	btExit = new Button(335, 490, resource->GetImage("exit-bt"));
	addGuiElement(btExit);
}

Menu::~Menu()
{
	delete btPlay;
	delete btExit;
	delete btCreate;
}

void Menu::onInputEvent(GuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	if(element == btExit)
	{
		switch (action)
		{
		case MOUSE_RELEASED_EVENT:
			setNext(EXIT);
			break;

		default:
			break;
		}
	}
	else if(element == btPlay)
	{
		switch (action)
		{
		case MOUSE_RELEASED_EVENT:
			//setNext(SIMULATE);
			setNext(GAMEPLAY);
			break;

		default:
			break;
		}
	}
	else if(element == btCreate)
	{
		switch (action)
		{
		case MOUSE_RELEASED_EVENT:
			setNext(CREATE_ARMY);
			break;

		default:
			break;
		}
	}
}

void Menu::Logic()
{
	updateGuiElements();
}

void Menu::Render()
{
	Game::getGlobalGame()->setBackgroundColor(255, 0, 0);

	imgBackground->DrawImage(Game::getGlobalGame()->getRenderer());
	imgStrategos->DrawImage( Game::getGlobalGame()->getRenderer(), 32, 8 );

	drawGuiElements();
}


void Menu::Clean()
{

}
