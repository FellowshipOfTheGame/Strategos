#include "Result.h"

#include "Global.h"
#include "Game.h"
#include "Image.h"
#include "CombatLog.h"

#include "SDL2_gfx/SDL2_gfxPrimitives.h"

Result::Result(STATE previous) :
	StateMachine(previous, RESULTS, RESULTS)
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();
	Resource *resource = Game::getGlobalGame()->getResourceMNGR();
	resource->AddImage("assets/base.gfx", "display-bg");
	//imagens de fundo
	imgBackground = resource->GetImage("display-bg");

	//botoes
	btn_Next = new Button(scrWidth * 0.85, scrHeight * 0.05, 150, 24, resource->GetImage("menu-bt"), "BT02");
	btn_Next->setText(resource->GetFont("jostix-14"), "Menu", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Next);

	original_log1 = Game::getGlobalGame()->getCombatLog(0);
	original_log2 = Game::getGlobalGame()->getCombatLog(1);

	original_log1->calculateGeneralLog();
	original_log2->calculateGeneralLog();

	normalizeRounds(original_log1->getGeneralLog(), original_log1->getGeneralLog());

//	log1->getGeneralLog().print();
//	log2->getGeneralLog().print();

//	printf("army 1 %d dano %d\n",Game::getGlobalGame()->getCombatLog(0)->getLogSize(),Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());
//	printf("army 2 %d dano %d\n",Game::getGlobalGame()->getCombatLog(1)->getLogSize(), Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());

	printf("compondo logs \n");

//	log1->print();
//	log2->print();
}

Result::~Result()
{
	delete btn_Next;
}

void Result::normalizeRounds(const CombatRound* l1, const CombatRound* l2)
{
    int timeMin = INT_MAX;
    int timeMax = 0;

    if (l1 && l2)
    {
        timeMin = std::min(l1->getFirstLoggedTime(), l2->getFirstLoggedTime());
        timeMax = std::max(l1->getLastLoggedTime(), l2->getLastLoggedTime());
    }
    else if (l1)
    {
        timeMin = l1->getFirstLoggedTime();
        timeMax = l1->getLastLoggedTime();
    }else{
        timeMin = l2->getFirstLoggedTime();
        timeMax = l2->getLastLoggedTime();
    }

    printf("Normal range: %d, %d\n", timeMin, timeMax);

    const double graphWidth = 900;
    const double graphHeight = 400;

    // Criar o mapa novo com no maximo graphWidth tempos para L1
    CombatRound reduced1;
    for (LogMap::const_iterator it = l1->getLog().begin(); it != l1->getLog().end(); ++it)
    {
        int time = it->first*(double)graphWidth/(double)timeMax;
        reduced1.addLog( time, it->second );
    }

    // Criar o mapa novo com no maximo graphWidth tempos para L2
    CombatRound reduced2;
    for (LogMap::const_iterator it = l2->getLog().begin(); it != l2->getLog().end(); ++it)
    {
        int time = it->first*(double)graphWidth/(double)timeMax;
        reduced2.addLog( time, it->second );
    }

    // Contar maximos para normalizar
    RoundData maximumOf1 = reduced1.getMaximumData();
    RoundData maximumOf2 = reduced2.getMaximumData();

    const double maxDmgDealt = std::max(maximumOf1.damageDealt, maximumOf2.damageDealt);
    const double maxDmgReceived = std::max(maximumOf1.damageReceived, maximumOf2.damageReceived);
    const int maxDeaths = std::max(maximumOf1.deaths, maximumOf2.deaths);
    const int maxKills = std::max(maximumOf1.kills, maximumOf2.kills);

    printf("MaxDmgDealt: %.2lf\n", maxDmgDealt);
    printf("MaxDmgReceived: %.2lf\n", maxDmgReceived);
    printf("MaxDeaths: %d\n", maxDeaths);
    printf("MaxKills: %d\n", maxKills);

    // Normalizando
    for (LogMap::const_iterator it = reduced1.getLog().begin(); it != reduced1.getLog().end(); ++it)
    {
        army0[DMG_DEALT].addDot(it->first, it->second.damageDealt/maxDmgDealt);
        army0[DMG_TAKEN].addDot(it->first, it->second.damageReceived/maxDmgDealt);
        army0[KILLS].addDot(it->first, it->second.kills/maxDmgDealt);
        army0[DEATHS].addDot(it->first, it->second.deaths/maxDmgDealt);
    }

    for (LogMap::const_iterator it = reduced2.getLog().begin(); it != reduced2.getLog().end(); ++it)
    {
        army1[DMG_DEALT].addDot(it->first, it->second.damageDealt/maxDmgDealt);
        army1[DMG_TAKEN].addDot(it->first, it->second.damageReceived/maxDmgDealt);
        army1[KILLS].addDot(it->first, it->second.kills/maxDmgDealt);
        army1[DEATHS].addDot(it->first, it->second.deaths/maxDmgDealt);
    }
}

void Result::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	if (element == btn_Next)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				setNext(MENU);
				break;

			default:
				break;
		}
	}
}

void Result::Logic()
{
	//verifica unit selecionada
	//verifica se um componente foi modificado
	updateGuiElements();
}

void Result::Render()
{
    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();

    Game::getGlobalGame()->setBackgroundColor(0, 0, 0);

    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
    army0[DMG_DEALT].drawGraph(renderer);

    SDL_SetRenderDrawColor( renderer, 255, 255, 0, 255 );
    army0[DMG_TAKEN].drawGraph(renderer);

    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
    army0[KILLS].drawGraph(renderer);

    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
    army0[DEATHS].drawGraph(renderer);

}

void Result::Clean()
{
}
