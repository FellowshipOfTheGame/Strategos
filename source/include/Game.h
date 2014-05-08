/*
 *  Strategos - Game.h
 *
 *  Esta classe eh um dos 'Controller' (@see MVC) do nosso software.
 *	Ela vai comunicar o mundo (@see World.h), que contem a simulacao
 *	do jogador com o exercito selecionado pelo Algoritmo Genetico,
 *  com a visualizacao, isto eh, os elementos graficos e a SDL (@see View.h).
 *
 *	Last update: 06/07/2012
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "View.h"

#include "Resource.h"
#include "Dictionary.h"
#include "GeneticAlgorithm.h"
#include "CombatLog.h"

#define CONFIG_FILE  "setup.cfg"

class Game
{
	private:
	    static Game *globalGame;

		std::fstream cfgFile;
		Resource resources;
		View *view;
		std::vector<Dictionary*> dict;
		Army *editingArmy;
		Army *armySim1, *armySim2;
		CombatLog *log1, *log2;
		bool run;
		int globalSeed;
        GeneticAlgorithm *algorithm[3];

		void loadDictionaries();

		Game();

	public:
		~Game();
		static Game* getGlobalGame(){
		    if (globalGame)
                return globalGame;
            return new Game();
        }

		bool isRunning();
		void setRunning(bool state);
		void update();

		SDL_Texture* getRendererFrameBuffer(){
			return view->getRendererFrameBuffer();
		}

		int getWidth() const
		{
			return view->getWidth();
		}

		int getHeight() const
		{
			return view->getHeight();
		}

        int getScreenBPP() const
        {
            return view->getBPP();
        }

		Resource *getResourceMNGR();
		Dictionary *getDictionary(int id);
		Dictionary *getDictionary(const std::string& name) const;
		int getNDictionary() const;

        void generateSprites(const Dictionary *d){
            for (unsigned int i = 0; i < dict.size(); ++i)
                if (dict[i] == d){
                    dict[i]->generateSprites();
                    return;
                }
        }

		void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b)
		{
			view->setBackgroundColor(r, g, b);
		}

		//Por enquanto vou colocar aqui
		unsigned long createNewGID();
		void readCFGKey(const char *key, void *info);

		Army* getEditingArmy() const;
		void setEditingArmy(Army* editingArmy);

		Army* getArmy1() const;
		void setArmy1(const std::string& str);
		void setArmy1(Army *a);

		Army* getArmy2() const;
		void setArmy2(const std::string& str);
		void setArmy2(Army *a);

        GeneticAlgorithm **getGA();

		CombatLog* getCombatLog(int i);

		SDL_Renderer *getRenderer();
};

#endif
