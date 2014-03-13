//
//  Play.cpp
//  Strategos
//
//  Created by guicc on 8/8/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#include "Play.h"
#include "Global.h"


Play::Play(STATE previous) :
		StateMachine(previous, GAMEPLAY, GAMEPLAY)
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();
	Font *fntEthnocentric;

	Resource *resource = Game::getGlobalGame()->getResourceMNGR();

	resource->AddImage("assets/ui.gfx", "bg_engaging");
	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddFont("assets/fonts.gfx", "jostix-14");
	resource->AddImage("assets/ui.gfx", "textfield-text");

	imgBackground = resource->GetImage("bg_engaging");

	fntEthnocentric = resource->GetFont("jostix-14");
	lbTitle = new Label("Choose your Destiny!!", fntEthnocentric, ColorRGB8::Red, ColorRGB8::White, "LB_TITLE");
	lbTitle->setPosition(350, 20);
	addGuiElement(lbTitle);

	btSimulate = new Button(scrWidth * 0.44, scrHeight * 0.85, 150, 24, resource->GetImage("menu-bt"), "BTTemp108");
	btSimulate->setText(fntEthnocentric, "SIMULATE", ColorRGB8::White, ColorRGB8::Black);
	addGuiElement(btSimulate);

	cbSelect1 = new ComboBox(120, 200, 150, 17, NULL, ColorRGB8::Green, ColorRGB8::White, "CB01");
	cbSelect1->setFont(fntEthnocentric);
	cbSelect1->addText("Player");
	cbSelect1->addText("CPU");
	addGuiElement(cbSelect1);

	cbSelect2 = new ComboBox(120, 400, 150, 17, NULL, ColorRGB8::Green, ColorRGB8::White, "CB02");
	cbSelect2->setFont(fntEthnocentric);
	cbSelect2->addText("Player");
	cbSelect2->addText("CPU");
	addGuiElement(cbSelect2);

	cbPlayer1 = new ComboBox(240, 200, 200, 17, NULL, ColorRGB8::White, ColorRGB8::Black, "CB-AP01");
	cbPlayer1->setFont(fntEthnocentric);
	addGuiElement(cbPlayer1);
	/*cbCPU1 = new ComboBox(240, 200, 200, 17, NULL, ColorRGB8::White, ColorRGB8::Black, "CB-AC01");
	cbCPU1->setFont(fntEthnocentric);
	cbCPU1->setVisible(false);
	addGuiElement(cbCPU1);*/

	cbPlayer2 = new ComboBox(240, 400, 200, 17, NULL, ColorRGB8::White, ColorRGB8::Black, "CB-AP02");
	cbPlayer2->setFont(fntEthnocentric);
	addGuiElement(cbPlayer2);
	/*cbCPU2 = new ComboBox(240, 400, 200, 17, NULL, ColorRGB8::White, ColorRGB8::Black, "CB-AC02");
	cbCPU2->setFont(fntEthnocentric);
	cbCPU2->setVisible(false);
	addGuiElement(cbCPU2);*/

	//Populate the combo-boxes with the name of the armies already created
	vector<string*> datFiles;
	Resource::getListOfFiles(datFiles, "assets/saves/", ".dat");
	for (unsigned int i = 0; i < datFiles.size(); ++i)
	{
		string aux = datFiles[i]->c_str();
		aux = aux.substr(0, aux.find_last_of('.'));
		aux = aux.substr(aux.find_last_of("\\/") + 1);
//		Army *armytemp = Army::loadArmy(aux.c_str());
//		if (armytemp)
//		{
			cbPlayer1->addText(aux.c_str());
			cbPlayer2->addText(aux.c_str());
//		}
//		delete armytemp;
	}
	/*Resource::getListOfFiles(datFiles, "assets/saves/GA/", ".dat");
	for (unsigned int i = 0; i < datFiles.size(); ++i)
	{
		string aux = datFiles[i]->c_str();
		aux = aux.substr(0, aux.find_last_of('.'));
		aux = aux.substr(aux.find_last_of("\\/") + 1);
		Army *armytemp = Army::loadArmy(aux.c_str());
		if (armytemp)
		{
			cbCPU1->addText(aux.c_str());
			cbCPU2->addText(aux.c_str());
		}
		delete armytemp;
	}*/
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
            GeneticAlgorithm **algorithm = Game::getGlobalGame()->getGA();

            const vector<Army*>& ar = algorithm[0]->getSelectedArmies();
            printf("NArmies: %u\n", ar.size());
            for (int i = 0; i < ar.size(); ++i)
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

	imgBackground->DrawImage(0, 0, renderer);

	drawGuiElements();
}

void Play::Clean()
{

}

bool previewArmy(const char *armyName)
{
	FILE *fp = NULL;
	char *armyFile;

	if (armyName != NULL)
	{
		armyFile = new char[64];
		strcpy(armyFile, "assets/saves/");
		strcat(armyFile, armyName);
		strcat(armyFile, ".dat");

		fp = fopen(armyFile, "r+");
		if (fp != NULL)
		{
			//Carrega info dos exercitos!!

			return true;
		}
	}

	return false;
}

