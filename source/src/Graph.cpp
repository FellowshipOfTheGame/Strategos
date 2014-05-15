#include "Graph.h"

Graph::Graph()
{

}

void Graph::drawGraph(SDL_Renderer *renderer)
{
    const int offX = 62;
    const int offY = 600;
    const int xscale = 8;
    const int yscale = 500;

    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );

    std::map<int, double>::const_iterator itNow = data.begin();
    std::map<int, double>::const_iterator itNext = data.begin();
    while ( ++itNext != data.end() )
    {
        SDL_RenderDrawLine( renderer, offX + itNow->first*xscale, offY - itNow->second*yscale,
                                      offX + itNext->first*xscale, offY - itNext->second*yscale );

        itNow = itNext;
    }



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
