/*
 *  Strategos - Game.cpp
 *
 */

#include <time.h>

#include "RandomEngine.h"
#include "Game.h"
#include "CombatLog.h"

#include "GeneticAlgorithm.h"
#include "NFGeneticAlgorithm.h"

// Telas
#include "Menu.h"
#include "Create_Army.h"
#include "Unit_Setup.h"
#include "Options.h"
#include "Simulation.h"
#include "Play.h"
#include "Result.h"

Game* Game::globalGame = 0;

Game::Game()
    : armySim1(nullptr), armySim2(nullptr), log1(nullptr), log2(nullptr), gameState(nullptr)
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

	run = true;
	editingArmy = nullptr;

	algorithm[0] = new NFGeneticAlgorithm(0);
	algorithm[1] = new NFGeneticAlgorithm(1);
	algorithm[2] = new NFGeneticAlgorithm(2);

	srand((unsigned int) time(nullptr));
	loadDictionaries();

    printf("Starting GA\n");
    clock_t time = clock();
    for (int i = 0; i < 1; ++i) // TODO: Alterar para 3 na versao final 1 apenas para agilizar debugs
    {
        algorithm[i]->initialize();
        algorithm[i]->run();
    }
    printf("GA time: %lfs\n", (clock()-time) / (double)CLOCKS_PER_SEC);
}

Game::~Game()
{
    delete gameState;

	if (editingArmy != nullptr)
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
}

bool Game::isRunning()
{
	return run;
}

void Game::setRunning(bool state)
{
	this->run = state;
}

bool Game::update()
{
    if (gameState == 0)
        gameState = new Menu(EXIT);
	//printf("STATE: %i, %i, %i\n", gameState->getPrevious(), gameState->getCurrent(), gameState->getNext());

	/** Carrega as configuracoes do novo estado */
	if (gameState->getCurrent() != gameState->getNext())
	{
		printf("Changing State...\n");
		gameState->setPrevious(gameState->getCurrent());
		gameState->setCurrent(gameState->getNext());

		//Como vamos criar um novo gameState, liberamos o anterior.
		StateMachine *tmpState = gameState;

		switch (gameState->getCurrent())
		{
			case NONE:
				printf("NONE\n");
				break;
			case MENU:
				gameState = new Menu(gameState->getPrevious());
				printf("MENU\n");
				break;
			case CREATE_ARMY:
				gameState = new Create_Army(gameState->getPrevious());
				printf("CREATE_ARMY\n");
				break;

			case UNIT_SETUP:
				gameState = new Unit_Setup(gameState->getPrevious());
				printf("UNIT_SETUP\n");
				break;

			case OPTIONS:
				gameState = new Options(gameState->getPrevious());
				printf("OPTIONS\n");
				break;

			case GAMEPLAY:
				gameState = new Play(gameState->getPrevious());
				printf("GAMEPLAY\n");
				break;

			case SIMULATE:
				gameState = new Simulation(gameState->getPrevious());
				printf("SIMULATE\n");
				break;

			case RESULTS:
				gameState = new Result(gameState->getPrevious());
				printf("RESULTS\n");
				break;

			case EXIT:
				return false;
				break;

			default:
				printf("Game State %i NOT HANDLED!\n", gameState->getCurrent());
				return false;
				break;
		}

		delete tmpState;
	}
	/*Inicia um novo ciclo de contagem de Ticks*/
	//if(!fps->paused)
	//	fps->StartCycle();

	/** Updates State*/
	//gameState->setPrevious(gameState->getCurrent());
	/** Captura a entrada de eventos
	 * Calcula a logica*/
	gameState->Input();
	gameState->Logic();

    // clear the screen

    view->DrawStart();

	/** Desenha o resultado na tela*/
	gameState->Render();

	view->DrawEnd();


    SDL_Delay(16);

	/*Segura alguns quadros ate que um certo intervalo de tempo tenha se completado*/
	//if(fps->GetTicks() < 1000/FRAMES_PER_SECOND)
	//	SDL_Delay( (1000/FRAMES_PER_SECOND) - fps->GetTicks());
	/** Updates State*/
	//gameState->setPrevious(gameState->getCurrent());
	//gameState->setCurrent(gameState->getNext());
	return true;
}

SDL_Texture* Game::getRendererFrameBuffer(){
    return view->getRendererFrameBuffer();
}

int Game::getWidth() const
{
    return view->getWidth();
}

int Game::getHeight() const
{
    return view->getHeight();
}

int Game::getScreenBPP() const
{
    return view->getBPP();
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

Dictionary *Game::getDictionary(const std::string& name) const
{
	for (unsigned int i = 0; i < dict.size(); ++i)
	{
		if (dict[i]->title == name)
		{
			return dict[i];
		}
	}

	return nullptr;
}

int Game::getNDictionary() const
{
    return dict.size();
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

Army* Game::getEditingArmy() const
{
	return editingArmy;
}

void Game::setEditingArmy(Army* editingArmy)
{
	this->editingArmy = editingArmy;
}

void Game::loadDictionaries()
{
	std::vector<std::string> dnyFiles;
	Resource::getListOfFiles(dnyFiles, "assets/dictionary/", ".dny");

	for (unsigned int i = 0; i < dnyFiles.size(); ++i)
	{
		Dictionary *dny = new Dictionary();
		dny->loadDictionary(dnyFiles[i].c_str(), resources);
		if (dny)
			dict.push_back(dny);
	}

	printf("== Loaded %u dictionaries ==\n", dict.size());
}

void Game::generateSprites(const Dictionary *d){
    for (unsigned int i = 0; i < dict.size(); ++i)
        if (dict[i] == d){
            dict[i]->generateSprites();
            return;
        }
}

void Game::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b)
{
    view->setBackgroundColor(r, g, b);
}

Army* Game::getArmy1() const
{
	return armySim1;
}

void Game::setArmy1(const std::string& str)
{
    Army *loaded = Army::loadArmy(str);

    setArmy1(loaded);
}

void Game::setArmy1(Army *a)
{
    if (a == nullptr)
    {
        delete armySim1;
        delete log1;

        armySim1 = nullptr;
        log1 = nullptr;

        return;
    }

	if (armySim1 != nullptr)
		delete armySim1;
	armySim1 = a;

	if (log1)
        delete log1;

    log1 = new CombatLog(armySim1->nUnits());
}

Army* Game::getArmy2() const
{
	return armySim2;
}

void Game::setArmy2(const std::string& str)
{
	Army *loaded = Army::loadArmy(str);

	setArmy2( loaded );
}

void Game::setArmy2(Army *a)
{
    if (a == nullptr)
    {
        delete armySim2;
        delete log2;

        armySim2 = nullptr;
        log2 = nullptr;

        return;
    }

	if (armySim2 != nullptr)
		delete armySim2;
	armySim2 = a;

	if (log2)
        delete log2;

    log2 = new CombatLog(armySim2->nUnits());
}

Algorithm** Game::getAlgorithm()
{
	return algorithm;
}

CombatLog* Game::getCombatLog(int i)
{
    if (i == 0)
        return log1;

	return log2;
}
