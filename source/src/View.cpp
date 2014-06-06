/*
 *  Strategos - View.cpp
 *
 */


#include "View.h"

 #include <stdio.h>
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
    SDL_GL_DeleteContext(glcontext);
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

int View::createWindow(int width, int height, int bpp, Uint32 flags)
{
    screenArea.x = 0;
	screenArea.y = 0;
	screenArea.w = width;
	screenArea.h = height;
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

	SDL_Window *window = SDL_CreateWindow( "Strategos - SDL2",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       width, height,
                                    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE );

    // Criar contexto
    glcontext = SDL_GL_CreateContext(window);

    int oglIdx = -1;
    int nRD = SDL_GetNumRenderDrivers();
    for(int i=0; i<nRD; i++)
    {
        SDL_RendererInfo info;
        if(!SDL_GetRenderDriverInfo(i, &info))
        {
            printf("DRIVER: %s\n", info.name);
            if(!strcmp(info.name, "opengl"))
            {
                oglIdx = i;
            }
        }
    }

    renderer = SDL_CreateRenderer(window, oglIdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);

    initOpenGL();

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

    glViewport( 0, 0, screenArea.w, screenArea.h );

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, screenArea.w, screenArea.h, 0.0f, 0.0f, 1.0f);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void View::SetViewport( int width, int height )
{
    // Atualizar view port
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    // Resetar matrizes de projecao
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // Voltar para model view
    glMatrixMode( GL_MODELVIEW );

    // Resetar matriz model view
    glLoadIdentity();
}

void View::DrawStart()
{
    glColor3f(1.0f,1.0f,1.0f);
    glClear( GL_COLOR_BUFFER_BIT );
}

void View::DrawEnd()
{
    SDL_RenderPresent(renderer);
}
