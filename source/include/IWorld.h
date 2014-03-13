#ifndef _IWORLD_H_
#define _IWORLD_H_

#include <list>

#include "Ship.h"

class worldData
{
    public:


    private:

};

class IWorld
{
    public:
        IWorld();
        virtual ~IWorld();

    private:
        /// Retorna uma lista de naves ordenada de acordo com a distancia ateh outra nave
        /// @from: Ship usada para comparar distancia
        virtual void getShipsByDistance(Ship *from, std::list<Ship*> &list) = 0;
};

#endif // _IWORLD_H_
