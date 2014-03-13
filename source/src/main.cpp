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
    freopen( "CON", "w", stdout ); //DEBUG -> Serve para mostrar os printf no console! (Na verdade manda os printfs para o arquivo CON)

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

	//abort(); //Quando usar o freeopen o jogo nao vai fechar direito... forcar o programa a fechar com abort!

	//system("PAUSE");
	return EXIT_SUCCESS;
}
