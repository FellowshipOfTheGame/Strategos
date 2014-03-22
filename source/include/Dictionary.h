/** Dicionario de naves - Kurama
 * Implementar como Arvore Binaria - facil acesso - sempre manter ordenado*/

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <string.h>
#include <fstream>
#include <vector>

#include "Image.h"
#include "Ship.h"

#include "Resource.h"

#define SPACIAL_UNIT 32.0

#define _ROTATION_FRAMES_ 36

class DictKey
{
    public:
        char name[16];
        shipBaseStats stats;
        unsigned int squadSize;
        int type;

        Image *shootGFX;
        Image *explosionGFX;
        Image **shipsGFX;

        DictKey(const DictKey *copy)
        {
            *this = *copy;
        }

        DictKey()
            : squadSize(0), shootGFX(0), shipsGFX(0)
        {
        }

        ~DictKey()
        {
        }


};

class Dictionary
{
    public:
        Dictionary();
        ~Dictionary();

        int loadDictionary(const char *inputFile, Resource &resource);
        const DictKey* getInfoFor(unsigned int pos) const;
        void generateSprites();

        string title;
        string description;

    private:
        DictKey* readAttribute(fstream &input, std::string &title, Resource &resource);

        vector<DictKey*> keys;
        Image *baseGFX;

        // Vetor de Vetores de imagens
        // shipsGFX[Type][Angulo%frames] eh um Image*
        Image ***shipsGFX;
};

#endif
