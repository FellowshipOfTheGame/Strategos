#include "Coordinates.h"

Coordinates::Coordinates()
{
    x = y = 0;
}

Coordinates::Coordinates(float a, float b)
{
    x = a;
    y = b;
}

Coordinates::Coordinates(const Coordinates *c)
{
    x = c->x;
    y = c->y;
}

// Como se somasse um vetor (a, b) � coordenada atual
Coordinates Coordinates::operator +(const Coordinates& coord) const
{
    return Coordinates(x+coord.x, y+coord.y);
}

Coordinates Coordinates::operator -(const Coordinates& coord) const
{
    return Coordinates(x-coord.x, y-coord.y);
}

void Coordinates::operator +=(const Coordinates& coord)
{
    x += coord.x;
    y += coord.y;
}

float Coordinates::distance(const Coordinates& c) const
{
    return (int)sqrt( pow((double)(x - c.x),2) + pow((double)(y - c.y),2) );
}

// tx, ty = target x, y
float Coordinates::distance(int tx, int ty) const
{
    return (int)sqrt( pow((double)(x - tx),2) + pow((double)(y - ty),2) );
}

float Coordinates::angleTo(const Coordinates &other) const
{
    return atan2(y - other.y, other.x - x);
}
