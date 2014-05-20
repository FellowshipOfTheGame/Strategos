#include "RandomEngine.h"

RandomEngine::RandomEngine(unsigned seed)
    : randE(seed)
{
}

RandomEngine::~RandomEngine()
{

}

int RandomEngine::nextInt()
{
    return (int) randE();
}

unsigned int RandomEngine::nextUInt()
{
    return randE();
}
