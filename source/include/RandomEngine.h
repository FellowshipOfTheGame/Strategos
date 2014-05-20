#ifndef _RANDOM_ENGINE_H_
#define _RANDOM_ENGINE_H_

#include <random>

class RandomEngine
{

    public:
        std::mt19937 randE;


        RandomEngine(unsigned seed);
        virtual ~RandomEngine();

        virtual int nextInt();
        virtual unsigned int nextUInt();
};

#endif
