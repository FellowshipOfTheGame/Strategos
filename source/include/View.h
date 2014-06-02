/*
 *  Strategos - View.h
 *
 *  Esta classe eh a 'View' (@see MVC) do nosso software.
 *	Ela vai renderizar o resultado da simulacao do jogador
 *	na tela.
 *	Alem disso, ela ira manter a maquina de estados principal
 *	do jogo (Menu, Options, CreateArmy, Gameplay)
 *
 *	Last update: 17/05/2014
 */

#ifndef _VIEW_H_
#define _VIEW_H_

#include <SDL.h>

class View
{
    public:
        View();
        ~View();

        /**
         * Cria uma janela e a armazena na superficie screen(SDL_Surface*)
         * Inicializa os componentes basicos da SDL e das bibliotecas
         * adicionais utilizadas: SDL_ttf
         */
        int createWindow(int width, int height, int bpp, Uint32 flags);

        SDL_Texture* getRendererFrameBuffer();
        SDL_Renderer* getRenderer();
        int getWidth() {return screenArea.w;}
        int getHeight() {return screenArea.h;}
        int getBPP() {return bpp;}

        void SetViewport( int width, int height );

        void DrawStart();
        void DrawEnd();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_GLContext glcontext;

        int bpp;
        Uint32 flag;

        SDL_Rect screenArea;

        void initOpenGL();

};

#endif
