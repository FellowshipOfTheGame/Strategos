/*
 *  Strategos - Resource.h
 *
 *  Esta eh uma estrutura para armazenar os assets.
 *	carregados do jogo.
 *  Dessa forma, assets repetidos nao sao carregados
 *  multiplas vezes, e economizados o tempo de load
 *  na transicao dos estados
 *
 *	Last update: 24/08/2012
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "Image.h"
#include "Font.h"

SDL_Texture *loadImageToMemory(const std::string &fileName);

class Resource
{
    public:
        std::map<std::string, Image*> images;
        std::map<std::string, Font*> fonts;

        Resource();
        ~Resource();

        // Retorna a imagem inserida (Se ela ja existe retorna a ocorrencia)
        // Ou NULL caso ocorra algum erro no carregamento
        Image* AddImage(const char *filepath, std::string key);
        Image* LoadImage(std::fstream &file, std::string key);
        void AddFont(const char *filepath, std::string key);
        void LoadFont(std::fstream &file, std::string key);

        Image* GetImage(std::string key);
        Font* GetFont(std::string key);

        // Retorna uma lista com o nome dos arquivos
        // O usuario se encarrega de liberar a memoria do vetor (das strings)
        static void getListOfFiles(std::vector<std::string*> &myVec, std::string insideDirectory, std::string extension);
};

#endif
