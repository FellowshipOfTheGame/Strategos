//
//  Random.h
//  Strategos
//
//  Created by guicc on 8/26/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#ifndef _RANDOM_H
#define _RANDOM_H_

#include <stdlib.h>
#include <time.h>

void initRand(int seed);
double drand();
int nrand(int n);
int nrand();

#endif
