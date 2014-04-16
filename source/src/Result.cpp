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

	//printf("army 1 %d dano %d\n",Game::getGlobalGame()->getCombatLog(0)->getLogSize(),Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());
	//printf("army 2 %d dano %d\n",Game::getGlobalGame()->getCombatLog(1)->getLogSize(), Game::getGlobalGame()->getCombatLog(0)->getTotalDamageTaken());

	printf("compondo logs \n");
	log1 = Game::getGlobalGame()->getCombatRound(0);
	printf ("%d\n", log1->getLog().size());
	log2 = Game::getGlobalGame()->getCombatRound(1);
	printf ("%d\n", log2->getLog().size());
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
	int x,y,i,j,k;
	int maxx=600,minx=200;
	int maxy=400,miny=200;
	int size =log1->getLog().size();
	for (i=1;i<size-1;i++)
	{
		x = (log1->getLog()[i]->getStep()- log1->getLog()[0]->getStep())/((log1->getLog()[0]->getStep()+ log1->getLog()[size]->getStep()));
		y = (log1->getLog()[i]->getRoundDamage()- log1->getLog()[0]->getRoundDamage())/((log1->getLog()[0]->getRoundDamage()+ log1->getLog()[size]->getRoundDamage()));

		x*=(maxx-minx);
		y*=(maxy-miny);
		SDL_Rect rLife;
		            rLife.x = x;
		            rLife.y = y;
		            rLife.w = 1;
		            rLife.h = 1;
		 SDL_SetRenderDrawColor(Game::getGlobalGame()->getRenderer(), 0,255,0, 200);
		 SDL_RenderFillRect(Game::getGlobalGame()->getRenderer(), &rLife);
		//lineRGBA(globalGame->getScreenSurface(),x,y,x+1,y+1,228, 228, 228, 250);
	}

}

void Result::Clean()
{
}
