#include "Result.h"

#include "Global.h"
#include "Game.h"
#include "Image.h"
#include "CombatLog.h"

#ifdef WIN32
#include "SDL2_gfx\SDL2_gfxPrimitives.h"
#endif

#ifdef __APPLE__
#include "SDL_gfx/SDL_gfxPrimitives.h"
#endif // _MAC_OS_

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

	log1 = Game::getGlobalGame()->getCombatLog(0)->getRegister();
	log2 = Game::getGlobalGame()->getCombatLog(1)->getRegister();

	printf("army 1 %d dano %d\n",Game::getGlobalGame()->getCombatLog(0)->getLogSize(),Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());
	printf("army 2 %d dano %d\n",Game::getGlobalGame()->getCombatLog(1)->getLogSize(), Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());

	printf("compondo logs \n");


	log1->print();
	log2->print();
}

Result::~Result()
{
	delete btn_Next;
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

	int x,y,i;
	int maxx=600,minx=200;
	int maxy=400,miny=200;
	int divy, xnext, ynext, val;
	int maxStep=0, maxDmg=0;
	CombatRoundItem *min,*max;
    SDL_Rect rFrame;
    rFrame.x = minx;
    rFrame.y = maxy;
    rFrame.w = maxx-minx;
    rFrame.h = maxy-miny;
    int size,pass;
    int sX, sY, tX, tY;

    Game::getGlobalGame()->setBackgroundColor(255, 255, 255);
    //imgBackground->DrawImage(renderer);
	drawGuiElements();

    SDL_SetRenderDrawColor(renderer, 200,200,200, 0);
    SDL_RenderFillRect(renderer, &rFrame);

    if (log1->getLog().size())
    {
    	maxStep = log1->getLog()[log1->getLog().size()-1]->getStep();
    	max = (*std::max_element(log1->getLog().begin(),log1->getLog().end(), CombatRoundItem::compareMaxDamage));
    	maxDmg = max->getRoundDamage();
    }
    if (log2->getLog().size())
    {
    	maxStep = (log2->getLog()[log2->getLog().size()-1]->getStep() < maxStep)?maxStep:log2->getLog()[log2->getLog().size()-1]->getStep();
    	max = (*std::max_element(log1->getLog().begin(),log1->getLog().end(), CombatRoundItem::compareMaxDamage));
    	int auxmaxDmg =  max->getRoundDamage();
    	if (auxmaxDmg > maxDmg)
    		maxDmg = auxmaxDmg;
    }

    sX = 400/(maxStep);
    sY = (-rFrame.h)/(maxDmg);

    tX = rFrame.x;
    tY = rFrame.y +rFrame.h;
    size = log1->getLog().size();
    if (size > 0)
    {
        min = log1->getLog()[0];
        max = log1->getLog()[size-1];

        SDL_SetRenderDrawColor(renderer, 255,0,0, 0);
        i =0;
        pass = 0;
        x = tX;
        y = tY;
        xnext = tX;
        ynext = tY;
        do
        {
			if ((pass+1) == log1->getLog()[i]->getStep())
			{
				xnext = (pass+1)*sX + tX;
				ynext = (log1->getLog()[i]->getRoundDamage()*sY) + tY;
				printf ("%d %d\n",x,xnext);
				i++;
			}
			else
			{
				xnext = (pass+1)*sX + tX;
				ynext = tY;
			}
            SDL_RenderDrawLine(renderer, x, y , xnext, ynext);
			x = xnext;
			y = ynext;
			pass = pass +1;
        } while ((i<size-1) && (pass < max->getStep()));
    }
    size = log2->getLog().size();
    if (size > 0)
    {
        min = log2->getLog()[0];
        max = log2->getLog()[size-1];

        SDL_SetRenderDrawColor(renderer, 0,0,255, 0);
        i =0;
        pass = 0;
        x = tX;
		y = tY;
		xnext = tX;
		ynext = tY;
        do
        {
			if ((pass+1) == log2->getLog()[i]->getStep())
			{
				xnext = (pass+1)*sX + tX;
				ynext = (log2->getLog()[i]->getRoundDamage()*sY) + tY;
				i++;
			}
			else
			{
				xnext = (pass+1)*sX + tX;
				ynext = tY;
			}
			SDL_RenderDrawLine(renderer, x, y , xnext, ynext);
			x = xnext;
			y = ynext;
			pass = pass +1;
        } while ((i<size-1) && (pass < max->getStep()));
    }
}

void Result::Clean()
{
}
