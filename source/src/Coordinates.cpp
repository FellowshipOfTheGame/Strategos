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

// Como se somasse um vetor (a, b) à coordenada atual
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

float Coordinates::angleTo(const Coordinates &other) const
{
    return atan2(y - other.y, other.x - x);
}
