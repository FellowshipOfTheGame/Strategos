/*
 *  Menu.cpp
 *  Strategos
 *
 *
 */

#include "Unit_Setup.h"

#include "Global.h"
#include "Game.h"

#include "Image.h"

//#include "GuiTactics.cpp"
//TODO arrumar blueprint
//TODO adicionar botao para mover unidades
Unit_Setup::Unit_Setup(STATE previous) :
		StateMachine(previous, UNIT_SETUP, UNIT_SETUP), rnd(1337)
{
    Game *game = Game::getGlobalGame();
	int scrWidth = game->getWidth();
	int scrHeight = game->getHeight();
	Font *fntEthnocentric;

	game->generateSprites(game->getEditingArmy()->getDictionary());

	Resource *resource = game->getResourceMNGR();
	resource->AddImage("assets/base.gfx", "blueprint1-bg");
	resource->AddImage("assets/base.gfx", "squadfocus-bg");
	resource->AddImage("assets/ui.gfx", "bg_unit_setup");
	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "back-bt");
	resource->AddImage("assets/ui.gfx", "confirm-bt");
	resource->AddImage("assets/army.gfx", "human-ships");
	fntEthnocentric = resource->GetFont("jostix-14");

	resource->AddFont("assets/fonts.gfx", "jostix-14");
	//imagens de fundo
	imgBackground = resource->GetImage("bg_unit_setup");
	squad_selec = resource->GetImage("squadfocus-bg");

	const Image *bprint = resource->GetImage("blueprint1-bg");
	blueprint = new ImageBox(scrWidth * 0.05, scrHeight * 0.15, bprint->getFrameWidth(), bprint->getFrameHeight(), bprint, 0, nullptr, "BX1");
	addGuiElement(blueprint);

	//label de titulo
	lbl_Geral = new Label("Unit Setup", fntEthnocentric, ColorRGB8::Green, ColorRGB8::White, "LB02");
	lbl_Geral->setPosition(scrWidth * 0.45, scrHeight * 0.05);
	addGuiElement(lbl_Geral);

	//botoes
	btn_Next = new Button(scrWidth * 0.60, scrHeight * 0.9, resource->GetImage("confirm-bt"));
	addGuiElement(btn_Next);

	btn_Back = new Button(scrWidth * 0.05, scrHeight * 0.9, resource->GetImage("back-bt"));
	addGuiElement(btn_Back);

	btn_Del = new Button(scrWidth * 0.5, scrHeight * 0.1, 150, 24, resource->GetImage("menu-bt"));
	btn_Del->setText(resource->GetFont("jostix-14"), "DELETE", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Del);

	btn_Move = new Button(scrWidth * 0.65, scrHeight * 0.1, 150, 24, resource->GetImage("menu-bt"));
	btn_Move->setText(resource->GetFont("jostix-14"), "MOVE", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Move);

	//Box de naves
	bx1 = new ImageBox(scrWidth * 0.1, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 0, nullptr, "BX1");
	addGuiElement(bx1);
	bx2 = new ImageBox(scrWidth * 0.2, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 1, nullptr, "BX1");
	addGuiElement(bx2);
	bx3 = new ImageBox(scrWidth * 0.3, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 2, nullptr, "BX1");
	addGuiElement(bx3);
	bx4 = new ImageBox(scrWidth * 0.4, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 3, nullptr, "BX1");
	addGuiElement(bx4);

	// adicionando box com as imagens das naves
	dct = game->getDictionary(0);
	char str[5];

    Army *editingArmy = game->getEditingArmy();
    editingArmy->setIsPlayer();
	printf("Army Squads: %d\n", editingArmy->nUnits());
	if (editingArmy->nUnits() < 1)
	{
		editingArmy->createUnit(0, new Coordinates(50 , 50));
	}

	for (unsigned int i = 0; i < editingArmy->nUnits(); i++)
	{
		sprintf(str, "%d", i);
		squad_number.push_back(new Label(str, fntEthnocentric, ColorRGB8::Green, ColorRGB8::White, "LB02"));
		squad_number[i]->setPosition(editingArmy->getUnitByID(i)->getAvgX() + blueprint->getX(), editingArmy->getUnitByID(i)->getAvgY() + blueprint->getY());
	}

	printf("tactic");
	list = new TacticList(800, 100);
	addGuiElement(list);
	printf("list\n");

	squad_focus = nullptr;
	put_squad = false;
	move_squad= false;
	squad_type = 0;

	list->setVisible(false);

	renderCombat = SDL_CreateTexture(Game::getGlobalGame()->getRenderer(), SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, TEAM_AREA_WIDTH, TEAM_AREA_HEIGHT);

    if(renderCombat == nullptr)
    {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        exit(-9);
    }
}

Unit_Setup::~Unit_Setup()
{
	delete btn_Next;
	delete btn_Back;
	delete btn_Del;
	delete btn_Move;
	delete lbl_Geral;

	delete list;

	delete bx1;
	delete bx2;
	delete bx3;
	delete bx4;

	delete blueprint;
}

void Unit_Setup::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	unsigned int i;

	TacticInfo info(0);
	if (element == btn_Back)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				setNext(CREATE_ARMY);
				return;

            default:
                break;
		}
	}
	else if (element == btn_Next)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				Army::saveArmy(Game::getGlobalGame()->getEditingArmy());
				setNext(MENU);
				return;

            default:
                break;
		}
	}
	else if (element == btn_Del)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				if (!put_squad && squad_focus->getType() != 0)
				{
					Game::getGlobalGame()->getEditingArmy()->removeUnit(squad_focus->getID());
					squad_focus = nullptr;
				}
				return;

            default:
                break;
		}
	}
	else if (element == btn_Move)
		{
			switch (action)
			{
				case MOUSE_RELEASED_EVENT:
					if (!move_squad)
					{
						move_squad = true;
					}
					return;

                default:
                break;
			}
		}
	else if ((element == bx1) && (!move_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				//put_squad = !put_squad;
				//squad_type = 0;
				return;

            default:
                break;
		}
	}
	else if ((element == bx2)&& (!move_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				put_squad = !put_squad;
				squad_type = 1;
				return;

            default:
                break;
		}
	}
	else if ((element == bx3)&& (!move_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				put_squad = !put_squad;
				squad_type = 2;
				return;

            default:
                break;
		}
	}
	else if ((element == bx4)&& (!move_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				put_squad = !put_squad;
				squad_type = 3;
				return;

            default:
                break;
		}
	}
	else if ((element == blueprint) && (put_squad)) // Adicionar uma unidade
	{
        if (action == MOUSE_RELEASED_EVENT)
        {
            char str[5];
            if (!move_squad)
            {
            	if (squad_type != 0)
            	{
                    Army *editingArmy = Game::getGlobalGame()->getEditingArmy();

                    editingArmy->createUnit(squad_type, new Coordinates(mouseX - blueprint->getX(), mouseY - blueprint->getY()));

                    squad_focus = editingArmy->getUnitAtIndex(editingArmy->nUnits() - 1);
                    squad_type = 0;
                    sprintf(str, "%d", editingArmy->nUnits());
                    squad_number.push_back(
                            new Label(str, Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"),
                                      ColorRGB8::Green, ColorRGB8::White, "LB02"));
                    squad_number[(squad_number.size() - 1)]->setPosition(squad_focus->getAvgX() + blueprint->getX(),
                                                                         squad_focus->getAvgY() + blueprint->getY());
                    list->setSquad(squad_focus);
            	}
            	put_squad = false;
            }
            else
            {
                Coordinates c(mouseX, mouseY);
                squad_focus->moveTo(c);

                put_squad = !put_squad;
                move_squad = !move_squad;
            }
            return;
        }
	}
	else
	{
        if (action == MOUSE_RELEASED_EVENT)
        {
            Army* editingArmy = Game::getGlobalGame()->getEditingArmy();
            for (i = 0; i < editingArmy->nUnits(); i++)
            {
                Unit* unit = editingArmy->getUnitByID(i);
                if (unit->hover(mouseX-blueprint->getX(), mouseY-blueprint->getY()))
                {
                    if (squad_focus == unit)
                    {
                        squad_focus = nullptr;
                        list->setVisible(false);
                    }
                    else
                    {
                        squad_focus = unit;
                        list->setVisible(true);
                        if (move_squad)
                        {
                            put_squad = true;
                        }
                    }
				   list->setSquad(squad_focus);
                   return;
                }
            }
		}
	}
}

void Unit_Setup::Logic()
{
	//verifica unit selecionada
	//verifica se um componente foi modificado
	updateGuiElements();

    // mover as naves
	Game::getGlobalGame()->getEditingArmy()->update(rnd);


}

void Unit_Setup::Render()
{
	int mouseX, mouseY;
	SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();


	Game::getGlobalGame()->setBackgroundColor(255, 0, 0);

	imgBackground->DrawImage(renderer);
	drawGuiElements();
	list->update();
	list->draw();

	if (squad_focus)
	{
		squad_selec->DrawImage(renderer,
		        squad_focus->getAvgX() + blueprint->getX() - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameWidth() / 2,
		        squad_focus->getAvgY() + blueprint->getY() - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameHeight() / 2);
	}

	// Desenhar army na tela
    SDL_SetRenderTarget(renderer, renderCombat);
        // Limpar textura
        SDL_SetTextureBlendMode(renderCombat, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
        SDL_SetTextureBlendMode(renderCombat, SDL_BLENDMODE_BLEND);

        Game::getGlobalGame()->getEditingArmy()->render();
    SDL_SetRenderTarget(renderer, 0);

    // Renderizar na posicao do blueprint
    SDL_Rect rect;
    rect.x = blueprint->getX();  rect.w = TEAM_AREA_WIDTH;
    rect.y = blueprint->getY();  rect.h = TEAM_AREA_HEIGHT;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
    SDL_RenderCopy( renderer, renderCombat, 0, &rect);

	if (put_squad)
	{
		SDL_GetMouseState(&mouseX, &mouseY);
		Game::getGlobalGame()->getResourceMNGR()->GetImage("human-ships")->DrawImage(renderer, mouseX, mouseY, squad_type);
	}

	for (unsigned int i = 0; i < squad_number.size(); i++)
	{
		squad_number[i]->setPosition(Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgX() + blueprint->getX(),
				Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgY() + blueprint->getY());
		squad_number[i]->draw();
	}
	//list->draw();
}

void Unit_Setup::Clean()
{

}
