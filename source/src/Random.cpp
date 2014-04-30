//
//  Random.cpp
//  Strategos
//
//  Created by guicc on 8/26/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#include "Random.h"

#include <stdlib.h>
#include <time.h>

void initRand(int seed)
{
	srand(seed);
}

double drand()
{
	return (double)( (double)rand() / (double)RAND_MAX );
}

int nrand(int n)
{
	return rand()%n;
}

int nrand()
{
	return rand();
}
