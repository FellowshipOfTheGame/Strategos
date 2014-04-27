#include "Result.h"
#include "Global.h"
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
	printf ("combat log size %d\n", log1->getLog().size());
	log2->print();
	printf ("combat log size %d\n", log2->getLog().size());
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
	Game::getGlobalGame()->setBackgroundColor(255, 0, 0);
	imgBackground->DrawImage(Game::getGlobalGame()->getRenderer());
	drawGuiElements();
	int x,y,i;
	int maxx=600,minx=200;
	int maxy=400,miny=200;
	int divy, xnext, ynext, val;
    SDL_Rect rFrame;
    rFrame.x = minx;
    rFrame.y = miny;
    rFrame.w = maxx;
    rFrame.h = maxy;
    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();
    SDL_SetRenderDrawColor(renderer, 0,255,0, 0);
    SDL_RenderFillRect(renderer, &rFrame);

    int size =log1->getLog().size();

    if (size > 0)
    {
        CombatRoundItem * min = log1->getLog()[0];
        CombatRoundItem * max = log1->getLog()[size-1];

        SDL_SetRenderDrawColor(renderer, 255,0,0, 0);

        xnext=log1->getLog()[0]->getStep();
        ynext=log1->getLog()[0]->getRoundDamage();
        for (i=0;i<size-1;i++)
        {
            x = (log1->getLog()[i]->getStep()- min->getStep())/(max->getStep() - min->getStep());
            divy = (max->getRoundDamage() - min->getRoundDamage());
            val = (log1->getLog()[i]->getRoundDamage() - min->getRoundDamage());
            y = val/((divy>0)?divy:100);

            SDL_RenderDrawLine(renderer,rFrame.x+x,rFrame.y+y,rFrame.x+xnext,rFrame.y+ynext);
            x = xnext;
            y = ynext;
        }
    }

	size = log2->getLog().size();

    if (size > 0)
    {
        CombatRoundItem * min = log2->getLog()[0];
        CombatRoundItem * max = log2->getLog()[size-1];

        SDL_SetRenderDrawColor(renderer, 0,0,255, 0);

        xnext=log2->getLog()[0]->getStep();
        ynext=log2->getLog()[0]->getRoundDamage();
        for (i=0;i<size-1;i++)
        {
            x = (log2->getLog()[i]->getStep()- min->getStep())/(max->getStep() - min->getStep());
            divy = (max->getRoundDamage() - min->getRoundDamage());
            val = (log2->getLog()[i]->getRoundDamage() - min->getRoundDamage());
            y = val/((divy>0)?divy:100);

            SDL_RenderDrawLine(renderer,rFrame.x+x,rFrame.y+y,rFrame.x+xnext,rFrame.y+ynext);
            x = xnext;
            y = ynext;
        }
    }

}

void Result::Clean()
{
}
