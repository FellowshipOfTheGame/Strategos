/*
 *  Strategos - Global.h
 *
 *  Arquivo para definicoes globais e inclusao de bibliotecas
 *  padrao utilizadas por todas as classes do projeto.
 *
 *	Last update: 25/08/2012
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <string>
#include <vector>
#include <cstdio>
#include <math.h>
#include "Game.h"

#define PI 3.14159265

#define FRAMES_PER_SECOND       25
#define RULE_PARAM              7
#define CONFIG_FILE             "setup.cfg"
#define SAVE_PATH               "assets/saves/"
#define SAVE_EXT                ".dat"

namespace ColorRGB8
{
    const SDL_Color White = {255, 255, 255};
    const SDL_Color Gray = {128, 128, 128};
    const SDL_Color Black = {0, 0, 0};
    const SDL_Color Red = {255, 0, 0};
    const SDL_Color Olive = {128, 128, 0};
    const SDL_Color Green = {0, 255, 0};
    const SDL_Color Blue = {0, 0, 255};
    const SDL_Color Yellow = {0, 255, 255};
}

// Usado para comparar cores nas fontes
bool compareColors(const SDL_Color color1, const SDL_Color color2);
    #define DIFERENTES  0
    #define IGUAIS      1

#endif
