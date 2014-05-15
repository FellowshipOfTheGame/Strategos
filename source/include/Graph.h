#ifndef GRAPH_H
#define GRAPH_H

#include <map>

#include "SDL.h"

class Graph
{
    public:
        Graph();

        void setPosition(int X, int Y);

        void drawGraph(SDL_Renderer *renderer, double xscale, double yscale);

        void addDot(int x, double fx);
        void setColor( Uint8 r, Uint8 g, Uint8 b, Uint8 a );

    private:
        int x, y;

        std::map<int, double> data;

        SDL_Color color;
};

#endif // GRAPH_H
