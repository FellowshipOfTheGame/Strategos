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

		fstream cfgFile;
		Resource resources;
		View *view;
		vector<Dictionary*> dict;
		Army *editingArmy;
		Army *armySim1, *armySim2;
		bool run;
		int globalSeed;
        GeneticAlgorithm *algorithm[3];

        CombatLog* combatLog[2];

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

		int getWidth()
		{
			return view->getWidth();
		}
		int getHeight()
		{
			return view->getHeight();
		}
        int getScreenBPP()
        {
            return view->getBPP();
        }
		Resource *getResourceMNGR();
		Dictionary *getDictionary(int id);
		Dictionary *getDictionary(string name);

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

		Army* getEditingArmy();
		void setEditingArmy(Army* editingArmy);

		Army* getArmy1();
		void setArmy1(string str);
		void setArmy1(Army *a);

		Army* getArmy2();
		void setArmy2(string str);
		void setArmy2(Army *a);

//        void setGA(GeneticAlgorithm *ga);
        GeneticAlgorithm **getGA();

		CombatLog* getCombatLog(int i);

		void setCombatLog(int i);

		SDL_Renderer *getRenderer();
};

#endif
