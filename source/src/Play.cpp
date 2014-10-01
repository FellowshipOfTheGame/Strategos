//
//  Play.cpp
//  Strategos
//
//  Created by guicc on 8/8/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#include "Play.h"

#include "Global.h"
#include "Game.h"
#include "Algorithm.h"

Play::Play(STATE previous) :
		StateMachine(previous, GAMEPLAY, GAMEPLAY)
{
	Resource *resource = Game::getGlobalGame()->getResourceMNGR();

	resource->AddImage("assets/ui.gfx", "bg_engaging");
	resource->AddImage("assets/ui.gfx", "start-bt");
	resource->AddFont("assets/fonts.gfx", "jostix-14");
	resource->AddFont("assets/fonts.gfx", "general-gui");
	resource->AddFont("assets/fonts.gfx", "general-gui-big");
	resource->AddImage("assets/ui.gfx", "textfield-text");

	imgBackground = resource->GetImage("bg_engaging");

	Font *fntEthnocentric = resource->GetFont("jostix-14");
	Font *fntGeneralBig = resource->GetFont("general-gui-big");
	Font *fntGeneral = resource->GetFont("general-gui");
	lbTitle = new Label("Choose your Destiny!!", fntEthnocentric, ColorRGB8::Red, ColorRGB8::White, "LB_TITLE");
	lbTitle->setPosition(350, 20);
	addGuiElement(lbTitle);

	btSimulate = new Button(350, 670, resource->GetImage("start-bt"));
	addGuiElement(btSimulate);

	cbSelect1 = new ComboBox(175, 200, nullptr, ColorRGB8::Red, ColorRGB8::Black);
	cbSelect1->setFont(fntGeneralBig);
	cbSelect1->addText("Player");
	cbSelect1->addText("CPU");
	addGuiElement(cbSelect1);

	cbSelect2 = new ComboBox(700, 200, nullptr, ColorRGB8::Green, ColorRGB8::Black);
	cbSelect2->setFont(fntGeneralBig);
	cbSelect2->addText("Player");
	cbSelect2->addText("CPU");
	addGuiElement(cbSelect2);

	cbPlayer1 = new ComboBox(200, 400, nullptr, ColorRGB8::Green, ColorRGB8::Black);
	cbPlayer1->setFont(fntGeneral);
	addGuiElement(cbPlayer1);

	cbPlayer2 = new ComboBox(720, 400, nullptr, ColorRGB8::Green, ColorRGB8::Black);
	cbPlayer2->setFont(fntGeneral);
	addGuiElement(cbPlayer2);

	//Populate the combo-boxes with the name of the armies already created
	std::vector<std::string> datFiles;
	Resource::getListOfFiles(datFiles, "assets/saves/", ".dat");
	for (unsigned int i = 0; i < datFiles.size(); ++i)
	{
		std::string& aux = datFiles[i];
		aux = aux.substr(0, aux.find_last_of('.'));
		aux = aux.substr(aux.find_last_of("\\/") + 1);

        cbPlayer1->addText(aux.c_str());
        cbPlayer2->addText(aux.c_str());
	}
}

Play::~Play()
{
	delete btSimulate;
	delete cbPlayer1;
	delete cbPlayer2;
	delete cbSelect1;
	delete cbSelect2;
}

void Play::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	if (btSimulate == element)
	{
		if (action == MOUSE_RELEASED_EVENT)
        {
            Algorithm **algorithm = Game::getGlobalGame()->getAlgorithm();

            const std::vector<Army*>& ar = algorithm[0]->getSelectedArmies();
            printf("NArmies: %lu\n", ar.size());
            for (unsigned int i = 0; i < ar.size(); ++i)
            {
                printf("Army: %d, Units: %d\n", i, ar[i]->nUnits());
            }

            if (cbSelect1->getSelectedIndex() == 0){
                Game::getGlobalGame()->setArmy1(cbPlayer1->getText());
            }else{
                Game::getGlobalGame()->setArmy1(algorithm[0]->higherFitnessArmy());
            }

            if (cbSelect2->getSelectedIndex() == 0){
                Game::getGlobalGame()->setArmy2(cbPlayer2->getText());
            }else{
                Game::getGlobalGame()->setArmy2(algorithm[0]->higherFitnessArmy());
            }

            if (Game::getGlobalGame()->getArmy1() == nullptr
                || Game::getGlobalGame()->getArmy2() == nullptr)
            {
                printf("ERROR: Army not loaded!\n");
                return;
            }

            printf("ARMY2: %p\n", Game::getGlobalGame()->getArmy2());

            /*if (algorithm){
                delete algorithm;
                algorithm = 0;
            }*/

            setNext(SIMULATE);
        }
	}
}

void Play::onKeyDownEvent(SDL_Keysym key)
{
	if (key.sym == SDLK_ESCAPE)
	{
		setNext(getPrevious());
	}
}

void Play::onKeyUpEvent(SDL_Keysym key)
{

}
void Play::onMouseDownEvent(Uint8 button)
{

}
void Play::onMouseUpEvent(Uint8 button)
{

}

void Play::Logic()
{
	if (cbSelect1->getSelectedIndex() == 0)
	{
		cbPlayer1->setVisible(true);
		//cbCPU1->setVisible(false);
	}
	else
	{
		cbPlayer1->setVisible(false);
		//cbCPU1->setVisible(true);
	}

	if (cbSelect2->getSelectedIndex() == 0)
	{
		cbPlayer2->setVisible(true);
		//cbCPU2->setVisible(false);
	}
	else
	{
		cbPlayer2->setVisible(false);
		//cbCPU2->setVisible(true);
	}

	updateGuiElements();
}

void Play::Render()
{
	SDL_Renderer *renderer = Game::getGlobalGame()->getRenderer();

	imgBackground->DrawImage(renderer, 0, 0);

	drawGuiElements();
}

void Play::Clean()
{

}

bool previewArmy(const char *armyName)
{
	FILE *fp = nullptr;
	char *armyFile;

	if (armyName != nullptr)
	{
		armyFile = new char[64];
		strcpy(armyFile, "assets/saves/");
		strcat(armyFile, armyName);
		strcat(armyFile, ".dat");

		fp = fopen(armyFile, "r+");
		if (fp != nullptr)
		{
			//Carrega info dos exercitos!!

			return true;
		}
	}

	return false;
}

