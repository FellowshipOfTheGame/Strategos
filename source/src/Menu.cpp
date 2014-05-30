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
	int scrWidth = globalGame->getWidth();
	int scrHeight = globalGame->getHeight();
	Font *fntEthnocentric;
	Image *imgTextField;

	Resource *resource = globalGame->getResourceMNGR();
	resource->AddImage("assets/ui.gfx", "bg_menu");
//	resource->AddImage("assets/ui.gfx", "bg_strategos");

	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "exit-bt");
	resource->AddImage("assets/ui.gfx", "play-bt");

	resource->AddFont("assets/fonts.gfx", "jostix-14");

	imgBackground = resource->GetImage("bg_menu");
//	imgStrategos = resource->GetImage("bg_strategos");

	btPlay = new Button(60, 645, 150, 24, resource->GetImage("menu-bt"));
	btPlay->setText(resource->GetFont("jostix-14"), "PLAY", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btPlay);

	btCreate = new Button(300, 645, 150, 24, resource->GetImage("menu-bt"));
	btCreate->setText(resource->GetFont("jostix-14"), "CREATE ARMY", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btCreate);

	btExit = new Button(760, 645, 148, 52, resource->GetImage("exit-bt"));
	addGuiElement(btExit);
}

Menu::~Menu()
{
	delete btPlay;
	delete btExit;
	delete btCreate;
}

void Menu::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
    printf("EVENTO!\n");
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
//	imgStrategos->DrawImage( Game::getGlobalGame()->getRenderer(), 32, 48 );

	drawGuiElements();
}


void Menu::Clean()
{

}
