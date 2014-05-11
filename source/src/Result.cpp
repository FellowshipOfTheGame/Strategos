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

	normalizeRounds(original_log1->getGeneralLog(), original_log1->getGeneralLog(), normalized_generalLog1, normalized_generalLog2);

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

void Result::normalizeRounds(const CombatRound* l1, const CombatRound* l2, const CombatRound& out1, const CombatRound& out2)
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

    double maxDmgDealt = std::max(l1->getMaximumData().damageDealt, l2->getMaximumData().damageDealt);
    double maxDmgReceived = std::max(l1->getMaximumData().damageReceived, l2->getMaximumData().damageReceived);
    double maxDeaths = std::max(l1->getMaximumData().deaths, l2->getMaximumData().deaths);
    double maxKills = std::max(l1->getMaximumData().kills, l2->getMaximumData().kills);

    printf("MaxDmgDealt: %.2lf\n", maxDmgDealt);
    printf("MaxDmgReceived: %.2lf\n", maxDmgReceived);
    printf("MaxDeaths: %d\n", maxDeaths);
    printf("MaxKills: %d\n", maxKills);
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
//	SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();
//
//	int i;
//	int maxx=600,minx=200;
//	int maxy=400,miny=200;
//	double x, y, xnext, ynext;
//	double maxStep=0, minStep=0, maxDmg=0;
//	CombatRoundItem *min,*max;
//    SDL_Rect rFrame;
//    rFrame.x = minx;
//    rFrame.y = maxy;
//    rFrame.w = maxx-minx;
//    rFrame.h = maxy-miny;
//    int size,pass;
//    double sX, sY, tX, tY;
//
//    Game::getGlobalGame()->setBackgroundColor(255, 255, 255);
//    //imgBackground->DrawImage(renderer);
//	drawGuiElements();
//
//    SDL_SetRenderDrawColor(renderer, 200,200,200, 0);
//    SDL_RenderFillRect(renderer, &rFrame);
//
//    if (log1->getLog().size())
//    {
//    	maxStep = log1->getLog()[log1->getLog().size()-1]->getStep();
//    	minStep = log1->getLog()[0]->getStep();
//    	max = (*std::max_element(log1->getLog().begin(),log1->getLog().end(), CombatRoundItem::compareMaxDamage));
//    	maxDmg = max->getRoundDamage();
//    } else {
//    	minStep = 99999;
//    }
//    if (log2->getLog().size())
//    {
//    	if ((log2->getLog()[log2->getLog().size()-1]->getStep() < maxStep))
//    	{
//    		maxStep = log2->getLog()[log2->getLog().size()-1]->getStep();
//    	}
//    	minStep = (log2->getLog()[0]->getStep() < minStep)?log2->getLog()[0]->getStep():minStep;
//    	max = (*std::max_element(log1->getLog().begin(),log1->getLog().end(), CombatRoundItem::compareMaxDamage));
//    	int auxmaxDmg =  max->getRoundDamage();
//    	if (auxmaxDmg > maxDmg)
//    		maxDmg = auxmaxDmg;
//    }
//
//    sX = (rFrame.w)/(maxStep- minStep);
//    printf ("%lf\n",sX);
//    sY = (-rFrame.h)/(maxDmg);
//
//    tX = rFrame.x;
//    tY = rFrame.y +rFrame.h;
//    size = log1->getLog().size();
//    if (size > 0)
//    {
//        min = log1->getLog()[0];
//        max = log1->getLog()[size-1];
//
//        SDL_SetRenderDrawColor(renderer, 255,0,0, 0);
//        i =0;
//        pass = min->getStep();
//        x = tX;
//        y = tY;
//        xnext = tX;
//        ynext = tY;
//        do
//        {
//			if ((pass+1) == log1->getLog()[i]->getStep())
//			{
//				xnext = (pass+1)*sX + tX;
//				ynext = (log1->getLog()[i]->getRoundDamage()*sY) + tY;
//				i++;
//			}
//			else
//			{
//				xnext = (pass+1)*sX + tX;
//				ynext = tY;
//			}
//            SDL_RenderDrawLine(renderer, x, y , xnext, ynext);
//			x = xnext;
//			y = ynext;
//			pass = pass +1;
//        } while ((i<size-1) && (pass < max->getStep()));
//    }
//    size = log2->getLog().size();
//    if (size > 0)
//    {
//        min = log2->getLog()[0];
//        max = log2->getLog()[size-1];
//
//        SDL_SetRenderDrawColor(renderer, 0,0,255, 0);
//        i =0;
//        pass = min->getStep();
//        x = tX;
//		y = tY;
//		xnext = tX;
//		ynext = tY;
//        do
//        {
//			if ((pass+1) == log2->getLog()[i]->getStep())
//			{
//				xnext = (pass+1)*sX + tX;
//				ynext = (log2->getLog()[i]->getRoundDamage()*sY) + tY;
//				i++;
//			}
//			else
//			{
//				xnext = (pass+1)*sX + tX;
//				ynext = tY;
//			}
//			SDL_RenderDrawLine(renderer, x, y , xnext, ynext);
//			x = xnext;
//			y = ynext;
//			pass = pass +1;
//        } while ((i<size-1) && (pass < max->getStep()));
//    }
}

void Result::Clean()
{
}
