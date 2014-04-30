/**
 * Novo projeto Strategos
 */

#include "Game.h"

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
