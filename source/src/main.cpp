/**
 * Novo projeto Strategos
 */

#include <time.h>

#include "Game.h"
#include "GeneticAlgorithm.h"

/*
    TODO:

    Arrumar exercitos durante o combate, o da direita precisa ser invertido

*/

using namespace std;

int main(int argc, char *argv[])
{
    printf ("criando game\n");

	Game* game = Game::getGlobalGame();

	printf ("while\n");
	while(game->isRunning())
	{
		//printf ("update\n");
		game->update();
	}

	printf("QUIT");

	delete game;

	printf("QUIT2");

	//system("PAUSE");
	return EXIT_SUCCESS;
}
