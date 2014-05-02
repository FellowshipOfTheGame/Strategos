#include "RandomEngine.h"

RandomEngine::RandomEngine(unsigned seed)
    : randE(seed)
{
}

int RandomEngine::nextInt()
{
    return (int) randE();
}
