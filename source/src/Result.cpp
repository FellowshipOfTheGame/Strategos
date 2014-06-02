#include "Result.h"

#include "Global.h"
#include "Game.h"
#include "CombatLog.h"

#include <limits.h>

#include <SDL_opengl.h>

Result::Result(STATE previous) :
	StateMachine(previous, RESULTS, RESULTS)
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();
	Resource *resource = Game::getGlobalGame()->getResourceMNGR();

	//imagens de fundo
	if (Game::getGlobalGame()->winner){
        resource->AddImage("assets/ui.gfx", "bg-win");
        imgBackground = resource->GetImage("bg-win");
    }
    else{
        resource->AddImage("assets/ui.gfx", "bg-lose");
        imgBackground = resource->GetImage("bg-lose");
    }

	//botoes
	btn_Next = new Button(scrWidth * 0.85, scrHeight * 0.05, 150, 24, resource->GetImage("menu-bt"));
	btn_Next->setText(resource->GetFont("jostix-14"), "Menu", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Next);

	// Legendas
	legendaDD = new Label("Army Damage Dealt", resource->GetFont("jostix-14"), ColorRGB8::Red, ColorRGB8::Black, "ldd" );
	legendaDR = new Label("Army Damage Received", resource->GetFont("jostix-14"), ColorRGB8::Yellow, ColorRGB8::Black, "ldd" );
	legendaK = new Label("Army Kills", resource->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::Black, "ldd" );
	legendaD = new Label("Army Deaths", resource->GetFont("jostix-14"), ColorRGB8::Blue, ColorRGB8::Black, "ldd" );
	legendaDD->setPosition(50, 620);
	legendaDR->setPosition(50, 620+16);
	legendaK->setPosition(50, 620+32);
	legendaD->setPosition(50, 620+48);
	addGuiElement(legendaDD);
	addGuiElement(legendaDR);
	addGuiElement(legendaK);
	addGuiElement(legendaD);

	original_log1 = Game::getGlobalGame()->getCombatLog(0);
	original_log2 = Game::getGlobalGame()->getCombatLog(1);

	// Gerar log somado de todas as unidades
	original_log1->calculateGeneralLog();
	original_log2->calculateGeneralLog();

	graphSteps = 100;
	graphW = 800;
	graphH = 500;
	graphX = 112;   // (windowW-graphW)/2
	graphY = 600;

	normalizeRounds(original_log1->getGeneralLog(), original_log1->getGeneralLog());

    // Cores dos graficos
    army0[DMG_DEALT].setColor( 255, 0, 0, 128 );    // Red
    army0[DMG_TAKEN].setColor( 255, 255, 0, 128 );  // Yellow
    army0[KILLS].setColor( 0, 255, 0, 128 );        // Green
    army0[DEATHS].setColor( 0, 0, 255, 128 );       // Blue

    army1[DMG_DEALT].setColor( 255, 0, 0, 128 );
    army1[DMG_TAKEN].setColor( 255, 255, 0, 128 );
    army1[KILLS].setColor( 0, 255, 0, 128 );
    army1[DEATHS].setColor( 0, 0, 255, 128 );

    army0[DMG_DEALT].setPosition(graphX, graphY);      army1[DMG_DEALT].setPosition(graphX, graphY);
    army0[DMG_TAKEN].setPosition(graphX, graphY);      army1[DMG_TAKEN].setPosition(graphX, graphY);
    army0[KILLS].setPosition(    graphX, graphY);      army1[KILLS].setPosition(    graphX, graphY);
    army0[DEATHS].setPosition(   graphX, graphY);      army1[DEATHS].setPosition(   graphX, graphY);

}

Result::~Result()
{
    Game::getGlobalGame()->setArmy1(nullptr);
    Game::getGlobalGame()->setArmy2(nullptr);

	delete btn_Next;
}

void Result::reduzir(const CombatRound* original, CombatRound& reduced, int steps, int timeMax)
{
    double ss = (double)steps/(double)timeMax;

    for (LogMap::const_iterator it = original->getLog().begin(); it != original->getLog().end(); ++it)
    {
        int time = it->first * ss;
        reduced.addLog( time, it->second );
    }
}

// Tratar Kills e Deaths
int Result::tratar(CombatRound& graph, int total_ships)
{
    // Garantir a existencia do ponto 0
    graph.addLog(0, RoundData());

    int sumKills = 0;
    for (LogMap::const_iterator it = graph.getLog().begin(); it != graph.getLog().end(); ++it)
    {
        RoundData modified(it->second);
        int deaths = modified.deaths;
        int kills = modified.kills;

        modified.deaths = total_ships - deaths;
        total_ships -= deaths;

        modified.kills += sumKills;
        sumKills += kills;

        graph.setLog( it->first, modified );
    }

    return sumKills;
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

    printf("Normal range: %d, %d - Step size: %.2lf\n", timeMin, timeMax, timeMax/(double)graphSteps);

    const int TotalShips1 = Game::getGlobalGame()->getArmy1()->getTotalShips();
    const int TotalShips2 = Game::getGlobalGame()->getArmy2()->getTotalShips();

    // Criar o mapa novo com no maximo GraphSteps para L1
    CombatRound reduced1;
    reduzir( l1, reduced1, graphSteps, timeMax );
    tratar(reduced1, TotalShips1);

    // Criar o mapa novo com no maximo GraphSteps para L2
    CombatRound reduced2;
    reduzir( l2, reduced2, graphSteps, timeMax );
    tratar(reduced2, TotalShips2);

    // Contar maximos para normalizar
    RoundData maximumOf1 = reduced1.getMaximumData();
    RoundData maximumOf2 = reduced2.getMaximumData();

    const double maxDmgDealt = std::max(maximumOf1.damageDealt, maximumOf2.damageDealt);
    const double maxDmgReceived = std::max(maximumOf1.damageReceived, maximumOf2.damageReceived);
    const double maxDamage = std::max(maxDmgDealt, maxDmgReceived);

    const double maxShips = std::max(TotalShips1, TotalShips2);
//    const double maxKills = std::max(maximumOf1.kills, maximumOf2.kills);

    printf("MaxDmgDealt: %.2lf\n", maxDmgDealt);
    printf("MaxDmgReceived: %.2lf\n", maxDmgReceived);
    printf("MaxShips: %.1lf\n", maxShips);
//    printf("MaxKills: %.1lf\n", maxKills);

    // Normalizando
    for (LogMap::const_iterator it = reduced1.getLog().begin(); it != reduced1.getLog().end(); ++it)
    {
        army0[DMG_DEALT].addDot(it->first,  it->second.damageDealt/maxDamage);
        army0[DMG_TAKEN].addDot(it->first,  it->second.damageReceived/maxDamage);
        army0[KILLS].addDot(it->first,      it->second.kills/maxShips);
        army0[DEATHS].addDot(it->first,     it->second.deaths/maxShips);
    }

    for (LogMap::const_iterator it = reduced2.getLog().begin(); it != reduced2.getLog().end(); ++it)
    {
        army1[DMG_DEALT].addDot(it->first,  it->second.damageDealt/maxDamage);
        army1[DMG_TAKEN].addDot(it->first,  it->second.damageReceived/maxDamage);
        army1[KILLS].addDot(it->first,      it->second.kills/maxShips);
        army1[DEATHS].addDot(it->first,     it->second.deaths/maxShips);
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

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    imgBackground->DrawImage(renderer);

    SDL_SetRenderDrawBlendMode( renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND );

    // Fazer estrutura do grafico
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

    // eixo X
    SDL_RenderDrawLine(renderer, graphX-10, graphY   , graphX+graphW+10, graphY);
    // eixo y
    SDL_RenderDrawLine(renderer, graphX   , graphY+10, graphX       , graphY-graphH-10);

    // Linhas guia
    SDL_SetRenderDrawColor( renderer, 64, 64, 64, 128 );
    for (int i = 0; i < graphSteps; ++i)
    {
        SDL_RenderDrawLine(renderer, graphX + i*(graphW/graphSteps), graphY+10,
                                     graphX + i*(graphW/graphSteps), graphY-graphH-10);
    }

    army0[DMG_DEALT].drawGraph(renderer, graphW/graphSteps, graphH);
    army0[DMG_TAKEN].drawGraph(renderer, graphW/graphSteps, graphH);
    army0[KILLS].drawGraph(renderer, graphW/graphSteps, graphH);
    army0[DEATHS].drawGraph(renderer, graphW/graphSteps, graphH);

    drawGuiElements();
}

void Result::Clean()
{
}
