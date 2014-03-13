/*
 *  Strategos - Game.cpp
 *
 */

//#include "time.h"
#include "Game.h"
#include "Random.h"
#include "CombatLog.h"

Game* Game::globalGame = 0;

Game::Game()
    : armySim1(0), armySim2(0)
{
	int screenWidth = 1024, screenHeight = 768, screenBPP = 0;
    globalGame = this;

	cfgFile.open(CONFIG_FILE);

	readCFGKey("Screen Width:", &screenWidth);
	readCFGKey("Screen Height:", &screenHeight);
	readCFGKey("Screen Bpp:", &screenBPP);
	readCFGKey("Global Seed:", &globalSeed);

	view = new View();
	view->createWindow(screenWidth, screenHeight, screenBPP, 0); // SDL_DOUBLEBUF | SDL_HWSURFACE
//
	run = true;
	editingArmy = NULL;

	algorithm[0] = new GeneticAlgorithm(0);
	algorithm[1] = new GeneticAlgorithm(1);
	algorithm[2] = new GeneticAlgorithm(2);
	combatLog.push_back(NULL);
	combatLog.push_back(NULL);

	initRand((unsigned int) time(NULL));
	loadDictionaries();

    for (int i = 0; i < 3; ++i){
        algorithm[i]->initialize();
        algorithm[i]->run();
    }
}

Game::~Game()
{
	if (editingArmy != NULL)
		delete editingArmy;

	while (dict.size())
	{
		delete dict[dict.size() - 1];
		dict.pop_back();
	}

	delete algorithm[0];
	delete algorithm[1];
	delete algorithm[2];

	globalGame = 0;

	delete view;
}

bool Game::isRunning()
{
	return run;
}

void Game::setRunning(bool state)
{
	this->run = state;
}

void Game::update()
{
	//	run = world->simulateStep();
	run = view->update();
}

Resource *Game::getResourceMNGR()
{
	return &resources;
}

//TODO: NÃO REMOVER ESTE METODO, NECESSARIO PARA OS COMBOBOXES
Dictionary* Game::getDictionary(int id)
{
	return dict[id];
}

Dictionary *Game::getDictionary(string name)
{
	for (unsigned int i = 0; i < dict.size(); ++i)
	{
		if (dict[i]->title == name)
		{
			return dict[i];
		}
	}

	return NULL;
}

SDL_Renderer *Game::getRenderer()
{
    return view->getRenderer();
}

void Game::readCFGKey(const char *key, void *info)
{

}

unsigned long Game::createNewGID()
{

	return 0;
}

Army* Game::getEditingArmy()
{
	return editingArmy;
}

void Game::setEditingArmy(Army* editingArmy)
{
	this->editingArmy = editingArmy;
}

void Game::loadDictionaries()
{
	vector<string*> dnyFiles;
	Resource::getListOfFiles(dnyFiles, "assets/dictionary/", ".dny");

	for (unsigned int i = 0; i < dnyFiles.size(); ++i)
	{
		Dictionary *dny = new Dictionary();
		dny->loadDictionary(dnyFiles[i]->c_str(), resources);
		if (dny)
			dict.push_back(dny);

		delete dnyFiles[i];
	}

	printf("== Loaded %u dictionaries ==\n", dict.size());
}

Army* Game::getArmy1()
{
	return armySim1;
}

void Game::setArmy1(string str)
{
	if (armySim1 != NULL)
		delete armySim1;
	armySim1 = Army::loadArmy(str);
}

void Game::setArmy1(Army *a)
{
	if (!a)
	{
		printf ("nulo\n");
	}
	if (armySim1 != NULL)
		delete armySim1;
	armySim1  = a;
}

Army* Game::getArmy2()
{
	return armySim2;
}

void Game::setArmy2(string str)
{
	if (armySim2 != NULL)
		delete armySim2;
	armySim2  = Army::loadArmy(str);
}

void Game::setArmy2(Army *a)
{
	if (!a)
	{
		printf ("nulo\n");
	}
	if (armySim2 != NULL)
		delete armySim2;
	armySim2  = a;
}

GeneticAlgorithm** Game::getGA()
{
	return algorithm;
}

CombatLog* Game::getCombatLog(int i)
{
	return combatLog[i];
}

void Game::setCombatLog(int i)
{
	CombatLog *x =combatLog[i];
	if (x!=NULL)
		delete x;
	combatLog[i] = new CombatLog();
}
