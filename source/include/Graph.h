#ifndef GRAPH_H
#define GRAPH_H

#include <map>

#include "SDL.h"

class Graph
{
    public:
        Graph();

        void drawGraph(SDL_Renderer *renderer);

        void addDot(int x, double fx);
        void setColor( Uint8 r, Uint8 g, Uint8 b, Uint8 a );

    private:
        std::map<int, double> data;

        double maxValue;
        SDL_Color color;
};

#endif // GRAPH_H
