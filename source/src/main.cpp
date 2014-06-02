/**
 * Novo projeto Strategos
 */

#include "Game.h"

using namespace std;

int main(int argc, char *argv[])
{
    printf ("Iniciando...\n");

	Game* game = Game::getGlobalGame();

	printf ("while\n");

	while( game->update() )
	{
	}

	printf("Exiting..");

	delete game;

	printf(".");

	//system("PAUSE");
	return EXIT_SUCCESS;
}
