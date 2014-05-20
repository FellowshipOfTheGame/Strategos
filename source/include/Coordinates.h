#ifndef _COORDINATES_H_
#define _COORDINATES_H_

#include <cmath>

class Coordinates
{
    public:
        float x;
        float y;

        Coordinates();
        Coordinates(float a, float b);
        Coordinates(const Coordinates *c);

        // Soma um vetor (a, b) a coordenada
        Coordinates operator +(const Coordinates& coord) const;

        Coordinates operator -(const Coordinates& coord) const;

        void operator +=(const Coordinates& coord);

        // Distancia ao quadrado [Nao efetua sqrt]
        float distance2(const Coordinates& c) const;

        float distance(const Coordinates& c) const;

        // tx, ty = target x, y
        float distance(int tx, int ty) const;

        float angleTo(const Coordinates &other) const;
};

#endif
