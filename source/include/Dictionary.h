/** Dicionario de naves - Kurama
 * Implementar como Arvore Binaria - facil acesso - sempre manter ordenado*/

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <string.h>
#include <fstream>
#include <vector>

#include "Image.h"
#include "Ship.h"
#include "SoundManager.h"

#include "Resource.h"

#define _ROTATION_FRAMES_ (64)

class DictKey
{
    public:
        char name[16];
        shipBaseStats stats;
        unsigned int squadSize;
        int type;
        int cost; //! Custo da unidade

        shipEffects gfx_sfx;

        DictKey(const DictKey *copy)
        {
            *this = *copy;
        }

        DictKey()
            : squadSize(0)
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
//        void generateSprites();

        std::string title;
        std::string description;

    private:
        DictKey* readAttribute(std::fstream &input, Resource &resource);

        std::vector<DictKey*> keys;
};

#endif
