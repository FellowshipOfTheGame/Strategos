#include "Graph.h"

#include "SDL2_gfxPrimitives.h"

Graph::Graph()
    : x(0), y(0)
{

}

void Graph::setPosition(int X, int Y)
{
    x = X;
    y = Y;
}

void Graph::drawGraph(SDL_Renderer *renderer)
{
    const int xscale = 8;
    const int yscale = 500;

    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );

    std::map<int, double>::const_iterator itNow = data.begin();
    std::map<int, double>::const_iterator itNext = data.begin();

    while ( ++itNext != data.end() )
    {
        const int x1 = x + itNow->first*xscale;
        const int y1 = y - itNow->second*yscale;
        SDL_RenderDrawLine( renderer, x1, y1,
                                      x + itNext->first*xscale, y - itNext->second*yscale );

        circleRGBA( renderer, x1, y1, 2, color.r, color.g, color.b, color.a );

        itNow = itNext;
    }

    circleRGBA( renderer, x + itNow->first*xscale, y - itNow->second*yscale,
                        2, color.r, color.g, color.b, color.a );

}

void Graph::addDot(int x, double fx){
    data[x] = fx;
}

void Graph::setColor( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
}
