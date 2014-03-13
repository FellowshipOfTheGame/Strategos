#include "Result.h"
#include "Global.h"
#include "Image.h"
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

	printf("army 1 %d\n",Game::getGlobalGame()->getCombatLog(0)->getSize());
	printf("army 2 %d\n",Game::getGlobalGame()->getCombatLog(1)->getSize());
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
}

void Result::Clean()
{
}
