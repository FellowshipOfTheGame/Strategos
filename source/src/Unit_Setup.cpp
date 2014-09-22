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

	Resource *resource = game->getResourceMNGR();
	resource->AddImage("assets/base.gfx", "blueprint1-bg");
	resource->AddImage("assets/base.gfx", "squadfocus-bg");
	resource->AddImage("assets/ui.gfx", "bg_unit_setup");
	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "back-bt");
	resource->AddImage("assets/ui.gfx", "confirm-bt");
	resource->AddImage("assets/army.gfx", "human-ships");
	Font *fntEthnocentric = resource->GetFont("jostix-14");

	resource->AddFont("assets/fonts.gfx", "jostix-14");
	//imagens de fundo
	imgBackground = resource->GetImage("bg_unit_setup");
	squad_selec = resource->GetImage("squadfocus-bg");

	// Tactic List
	list = new TacticList(800, 110);
	list->setVisible(false);
	addGuiElement(list);

	// Blue Print
	const Image *bprint = resource->GetImage("blueprint1-bg");
	blueprint = new ImageBox(scrWidth * 0.05, scrHeight * 0.18, bprint->getFrameWidth(), bprint->getFrameHeight(), bprint, 0, nullptr);
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

	// adicionando box com as imagens das naves
	dct = game->getDictionary(0);

    //Box de naves
	bx1 = new ImageBox(scrWidth * 0.1, scrHeight * 0.12, 128, 90, dct->getInfoFor(0)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx1);
	bx2 = new ImageBox(scrWidth * 0.2, scrHeight * 0.12, 128, 70, dct->getInfoFor(1)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx2);
	bx3 = new ImageBox(scrWidth * 0.3, scrHeight * 0.12, 128, 90, dct->getInfoFor(2)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx3);
	bx4 = new ImageBox(scrWidth * 0.4, scrHeight * 0.12, 128, 90, dct->getInfoFor(3)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx4);

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

    st_box = new StatusBox(0, 0, nullptr, nullptr, "Statusbox");
	st_box->update(0, 0, dct->getInfoFor(0));
	st_box->setVisible(false);
	addGuiElement(st_box);

	squad_focus = nullptr;
	put_squad = false;
	move_squad= false;
	squad_type = 0;


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
			    if (squad_focus == nullptr)
                    return;

				if (!put_squad && squad_focus->getType() != 0)
				{
				    delete squad_number[squad_focus->getID()];
					squad_number.erase(squad_number.begin() + squad_focus->getID());

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
	else if (element == blueprint) // Adicionar uma unidade
	{
        if (action == MOUSE_RELEASED_EVENT)
        {
            Army *editingArmy = Game::getGlobalGame()->getEditingArmy();

            if (put_squad)
            {
                char str[5];
                if (!move_squad)
                {
                    if (squad_type != 0)
                    {

                        editingArmy->createUnit(squad_type, Coordinates(mouseX - blueprint->getX(), mouseY - blueprint->getY()));

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
                    Coordinates c(mouseX- blueprint->getX(), mouseY- blueprint->getY());
                    squad_focus->moveTo(c);

                    put_squad = !put_squad;
                    move_squad = !move_squad;
                }
            }
            else
            {
                for (size_t i = 0; i < editingArmy->nUnits(); i++)
                {
                    Unit* unit = editingArmy->getUnitByID(i);
                    if (unit->hover(mouseX-blueprint->getX(), mouseY-blueprint->getY()))
                    {
                        if ( move_squad )
                        {
                            put_squad = true;
                            squad_focus = unit;
                            list->setVisible(true);
                        }
                        else
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
                            }
                        }
                       list->setSquad(squad_focus);
                       return;
                    }
                }

                squad_focus = nullptr;
                list->setSquad(squad_focus);
                list->setVisible(false);
            }
        }
	}
}

void Unit_Setup::Logic()
{
    int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();

    if (bx1->hover())
	{
		st_box->update(scrWidth * 0.8, scrHeight * 0.2, dct->getInfoFor(0));
		st_box->setVisible(true);
	}
	else if (bx2->hover())
	{
		st_box->update(scrWidth * 0.8, scrHeight * 0.2, dct->getInfoFor(1));
		st_box->setVisible(true);
	}
	else if (bx3->hover())
	{
		st_box->update(scrWidth * 0.8, scrHeight * 0.2, dct->getInfoFor(2));
		st_box->setVisible(true);
	}
	else if (bx4->hover())
	{
		st_box->update(scrWidth * 0.8, scrHeight * 0.2, dct->getInfoFor(3));
		st_box->setVisible(true);
	}
	else
    {
		st_box->setVisible(false);
	}

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

    // Mostrar GUI
    drawGuiElements();
	list->update();
	list->draw();

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

    // Elemento selecionado
	if (squad_focus)
	{
		squad_selec->DrawImage(renderer,
		        squad_focus->getAvgX() + blueprint->getX() - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameWidth() / 2,
		        squad_focus->getAvgY() + blueprint->getY() - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameHeight() / 2);
	}

	// Unit sendo adicionada
	if (put_squad)
	{
		SDL_GetMouseState(&mouseX, &mouseY);
		dct->getInfoFor(squad_type)->gfx_sfx.shipIdle->DrawImage(renderer, mouseX, mouseY, 0);
	}

	// Numero das unidades
	for (unsigned int i = 0; i < squad_number.size(); i++)
	{
		squad_number[i]->setPosition(Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgX() + blueprint->getX(),
				Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgY() + blueprint->getY());
		squad_number[i]->draw();
	}
}

void Unit_Setup::Clean()
{

}
