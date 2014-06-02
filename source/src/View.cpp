/*
 *  Strategos - View.cpp
 *
 */

#include "View.h"
#include "Global.h"

#include <SDL_ttf.h>
#include <SDL_opengl.h>


View::View()
{
	//Inicia a maquina de estados do jogo
	window = 0;
	renderer = 0;
}

View::~View()
{
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
//	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL_ttf - %s\n", SDL_GetError());
		return -1;
	}

    SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowTitle(window, "Strategos - SDL2");

    initOpenGL();

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

void View::initOpenGL()
{
    glEnable( GL_TEXTURE_2D );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glViewport( 0, 0, 640, 480 );

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho(0.0f, 640, 480, 0.0f, -1.0f, 1.0f);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

int Display_SetViewport( int width, int height )
{
    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return 1;
}

void View::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void View::DrawStart()
{
    SDL_RenderClear(renderer);
}

void View::DrawEnd()
{
    SDL_RenderPresent(renderer);
}
