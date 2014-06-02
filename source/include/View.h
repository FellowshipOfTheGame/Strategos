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

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "StateMachine.h"

class View
{
    public:
        View();
        ~View();

        int createWindow(int width, int height, int bpp, Uint32 flags);
        void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b);

        SDL_Texture* getRendererFrameBuffer();
        SDL_Renderer* getRenderer();
        int getWidth() {return screenArea.w;}
        int getHeight() {return screenArea.h;}
        int getBPP() {return bpp;}

        void DrawStart();
        void DrawEnd();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;

        int bpp;
        Uint32 flag;

        SDL_Rect screenArea;

        void initOpenGL();

};

#endif
