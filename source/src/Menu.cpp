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

	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "exit-bt");
	resource->AddImage("assets/ui.gfx", "play-bt");
	resource->AddImage("assets/ui.gfx", "combo-cmb");
	resource->AddImage("assets/ui.gfx", "textfield-text");

	resource->AddFont("assets/fonts.gfx", "jostix-14");

	imgBackground = resource->GetImage("bg_menu");

	btPlay = new Button(scrWidth*0.75, scrHeight*0.2, 150, 24, resource->GetImage("menu-bt"), "BT01");
	btPlay->setText(resource->GetFont("jostix-14"), "PLAY", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btPlay);

	btCreate = new Button(scrWidth*0.75, scrHeight*0.3, 150, 24, resource->GetImage("menu-bt"), "BT01");
	btCreate->setText(resource->GetFont("jostix-14"), "CREATE ARMY", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btCreate);

	btExit = new Button(scrWidth*0.75, scrHeight*0.4, 148, 52, resource->GetImage("exit-bt"), "BT00");
	addGuiElement(btExit);

	fntEthnocentric = resource->GetFont("jostix-14");
	lbLabel = new Label("I'm a Label", fntEthnocentric, ColorRGB8::Green, ColorRGB8::White, "LB01");
	lbLabel->setPosition(400, 200);
	addGuiElement(lbLabel);

	imgTextField = resource->GetImage("textfield-text");
	tfTextField = new TextField(400, 300, 200, 17, imgTextField, ColorRGB8::White, "TF01");
	tfTextField->setFont(fntEthnocentric);
	addGuiElement(tfTextField);

	cbComboBox = new ComboBox(400, 400, 150, 17, globalGame->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Green, ColorRGB8::White, "CB01");
	cbComboBox->setFont(fntEthnocentric);
	cbComboBox->addText("texto 1");
	cbComboBox->addText("texto 2");
	cbComboBox->addText("texto 3");
	cbComboBox->addText("texto 4");
	cbComboBox->addText("texto 5");
	addGuiElement(cbComboBox);
}

Menu::~Menu()
{
	delete btPlay;
	delete btExit;
	delete btCreate;
	delete lbLabel;
	delete cbComboBox;
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

	drawGuiElements();
}

void Menu::Clean()
{

}
