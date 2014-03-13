#ifndef _COORDINATES_H_
#define _COORDINATES_H_

#include <cmath>

class Coordinates
{
public:

    float x;
    float y;

    Coordinates()
    {
        x = y = 0;
    }

    Coordinates(float a, float b)
    {
        x = a;
        y = b;
    }

    Coordinates(Coordinates *c)
    {
        x = c->x;
        y = c->y;
    }

// Como se somasse um vetor (a, b) à coordenada atual
    Coordinates operator +(const Coordinates& coord) const
    {
        return Coordinates(x+coord.x, y+coord.y);
    }

    Coordinates operator -(const Coordinates& coord) const
    {
        return Coordinates(x-coord.x, y-coord.y);
    }

    void operator +=(const Coordinates& coord)
    {
        x += coord.x;
        y += coord.y;
    }

    float distance(const Coordinates& c) const
    {
        return (int)sqrt( pow((double)(x - c.x),2) + pow((double)(y - c.y),2) );
    }

    // tx, ty = target x, y
    float distance(int tx, int ty) const
    {
        return (int)sqrt( pow((double)(x - tx),2) + pow((double)(y - ty),2) );
    }

    float angleTo(const Coordinates &other) const
    {
        return atan2(y - other.y, other.x - x);
    }
};

#endif
