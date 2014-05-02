#ifndef _RANDOM_ENGINE_H_
#define _RANDOM_ENGINE_H_

#include <random>

class RandomEngine
{
    std::mt19937 randE;

    public:
        RandomEngine(unsigned seed);

        int nextInt();
};

#endif
