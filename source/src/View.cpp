/*
 *  Strategos - View.cpp
 *
 */

#include "View.h"
#include "Menu.h"
#include "Create_Army.h"
#include "Unit_Setup.h"
#include "Options.h"
#include "Simulation.h"
#include "Play.h"
#include "Result.h"
#include "global.h"

View::View()
{
	//Inicia a maquina de estados do jogo
	gameState = 0;
	window = 0;
	renderer = 0;
}

View::~View()
{
	delete gameState;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

SDL_Renderer* View::getRenderer(){
    return renderer;
}

SDL_Texture* View::getRendererFrameBuffer(){
    return 0;
}

/**
 * Cria uma janela e a armazena na superficie screen(SDL_Surface*)
 * Inicializa os componentes basicos da SDL e das bibliotecas
 * adicionais utilizadas: SDL_ttf
 */
int View::createWindow(int width, int height, int bpp, Uint32 flags)
{
    this->bpp = bpp;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL_ttf - %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow("Strategos - SDL2",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          width, height,
                          0); // SDL_WINDOW_OPENGL

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	screenArea.x = 0;
	screenArea.y = 0;
	screenArea.w = width;
	screenArea.h = height;

	if (window == nullptr)
	{
		fprintf(stderr, "Couldn't create the window - %d x %d x %d - %d\n", width, height, bpp, flags);
		return -1;
	}

	return 0;
}

bool View::update()
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
    SDL_RenderClear(renderer);

	/** Desenha o resultado na tela*/
	gameState->Render();

    SDL_RenderPresent(renderer);

    SDL_Delay(16);

	/*Segura alguns quadros ate que um certo intervalo de tempo tenha se completado*/
	//if(fps->GetTicks() < 1000/FRAMES_PER_SECOND)
	//	SDL_Delay( (1000/FRAMES_PER_SECOND) - fps->GetTicks());
	/** Updates State*/
	//gameState->setPrevious(gameState->getCurrent());
	//gameState->setCurrent(gameState->getNext());
	return true;
}

void View::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}
