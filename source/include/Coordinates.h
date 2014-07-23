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
        inline float distance2(const Coordinates& c) const{
            const float A = x - c.x;
            const float B = y - c.y;
            return A*A + B*B;
        }

        inline float distance(const Coordinates& c) const{
            const float A = x - c.x;
            const float B = y - c.y;
            return sqrtf( A*A + B*B );
        }

        // tx, ty = target x, y
        inline float distance(int tx, int ty) const{
            const float A = x - tx;
            const float B = y - ty;
            return sqrtf( A*A + B*B );
        }

        float angleTo(const Coordinates &other) const;
};

#endif
