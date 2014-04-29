/*
 *  Menu.cpp
 *  Strategos
 *
 *
 */

#include "Unit_Setup.h"
#include "Global.h"
#include "Image.h"
//#include "GuiTactics.cpp"
//TODO arrumar blueprint
//TODO adicionar botao para mover unidades
Unit_Setup::Unit_Setup(STATE previous) :
		StateMachine(previous, UNIT_SETUP, UNIT_SETUP)
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();
	Font *fntEthnocentric;

	Game::getGlobalGame()->generateSprites(Game::getGlobalGame()->getEditingArmy()->getDictionary());

	Resource *resource = Game::getGlobalGame()->getResourceMNGR();
	resource->AddImage("assets/ui.gfx", "bg_unit_setup_1");

	resource->AddImage("assets/base.gfx", "blueprint1-bg");
	resource->AddImage("assets/base.gfx", "squadfocus-bg");

	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "exit-bt");
	resource->AddImage("assets/ui.gfx", "play-bt");
	resource->AddImage("assets/army.gfx", "human-ships");
	fntEthnocentric = resource->GetFont("jostix-14");

	resource->AddFont("assets/fonts.gfx", "jostix-14");
	//imagens de fundo
	imgBackground = resource->GetImage("bg_unit_setup_1");
	squad_selec = resource->GetImage("squadfocus-bg");
	blueprint = new ImageBox(scrWidth * 0.05, scrHeight * 0.15, resource->GetImage("blueprint1-bg")->getFrameWidth(),
	        resource->GetImage("blueprint1-bg")->getFrameHeight(), resource->GetImage("blueprint1-bg"), 0, nullptr, "BX1");
	addGuiElement(blueprint);

	//label de titulo
	lbl_Geral = new Label("Unit Setup", fntEthnocentric, ColorRGB8::Green, ColorRGB8::White, "LB02");
	lbl_Geral->setPosition(scrWidth * 0.45, scrHeight * 0.05);
	addGuiElement(lbl_Geral);

	//botoes
	btn_Next = new Button(scrWidth * 0.85, scrHeight * 0.05, 150, 24, resource->GetImage("menu-bt"), "BT02");
	btn_Next->setText(resource->GetFont("jostix-14"), "PLAY", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Next);

	btn_Back = new Button(scrWidth * 0.05, scrHeight * 0.05, 150, 24, resource->GetImage("menu-bt"), "BT03");
	btn_Back->setText(resource->GetFont("jostix-14"), "BACK", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Back);

	btn_Del = new Button(scrWidth * 0.05, scrHeight * 0.9, 150, 24, resource->GetImage("menu-bt"), "BT03");
	btn_Del->setText(resource->GetFont("jostix-14"), "DELETE", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Del);

	btn_Move = new Button(scrWidth * 0.2, scrHeight * 0.9, 150, 24, resource->GetImage("menu-bt"), "BT03");
	btn_Move->setText(resource->GetFont("jostix-14"), "MOVE", ColorRGB8::White, ColorRGB8::White);
	addGuiElement(btn_Move);

	//Box de naves
	bx1 = new ImageBox(scrWidth * 0.3, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 0, nullptr, "BX1");
	addGuiElement(bx1);
	bx2 = new ImageBox(scrWidth * 0.4, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 1, nullptr, "BX1");
	addGuiElement(bx2);
	bx3 = new ImageBox(scrWidth * 0.5, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 2, nullptr, "BX1");
	addGuiElement(bx3);
	bx4 = new ImageBox(scrWidth * 0.6, scrHeight * 0.1, 50, 50, resource->GetImage("human-ships"), 3, nullptr, "BX1");
	addGuiElement(bx4);
	// adicionando box com as imagens das naves
	dct = Game::getGlobalGame()->getDictionary(0);
	char str[5];
	printf("Army Squads: %d\n", Game::getGlobalGame()->getEditingArmy()->nUnits());
	for (unsigned int i = 0; i < Game::getGlobalGame()->getEditingArmy()->nUnits(); i++)
	{
		sprintf(str, "%d\n", i);
		squad_number.push_back(new Label(str, fntEthnocentric, ColorRGB8::Green, ColorRGB8::White, "LB02"));
		squad_number[i]->setPosition(Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgX() + blueprint->getX(),
		        Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->getAvgY() + blueprint->getY());
	}
	printf("tactic");
	list = new TacticList(800, 100);
	addGuiElement(list);
	printf("list\n");
	//else
	squad_focus = nullptr;
	put_squad = false;
	move_squad= false;
	squad_type = 0;
	//	img1 =
	list->setVisible(false);
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

	/*delete tct_1;
	 delete tct_2;
	 delete tct_3;
	 delete tct_4;
	 delete tct_5;*/

	delete blueprint;
}

void Unit_Setup::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	int mouseX, mouseY;
	unsigned int i;

	TacticInfo info(0);
	if (element == btn_Back)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				setNext(CREATE_ARMY);
				break;

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
				break;

			default:
				break;
		}
	}
	else if (element == btn_Del)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				if (!put_squad)
				{
					Game::getGlobalGame()->getEditingArmy()->removeUnit(squad_focus->getID());
					squad_focus = nullptr;
				}
				break;

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
					break;

				default:
					break;
			}
		}
	else if ((element == bx1) && (!move_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				put_squad = !put_squad;
				squad_type = 0;
				break;

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
				break;

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
				break;

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
				break;

			default:
				break;
		}
	}
	else if ((element == blueprint) && (put_squad))
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:

				char str[5];
				SDL_GetMouseState(&mouseX, &mouseY);
				if ((put_squad) && (!move_squad))
				{
				    Army *editingArmy = Game::getGlobalGame()->getEditingArmy();

                    editingArmy->createUnit(squad_type, new Coordinates(mouseX - blueprint->getX(), mouseY - blueprint->getY()));

                    squad_focus = editingArmy->getUnitAtIndex(editingArmy->nUnits() - 1);
                    put_squad = false;
                    squad_type = 0;
                    sprintf(str, "%d\n", editingArmy->nUnits());
                    squad_number.push_back(
                            new Label(str, Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"),
                                      ColorRGB8::Green, ColorRGB8::White, "LB02"));
                    squad_number[(squad_number.size() - 1)]->setPosition(squad_focus->getAvgX() + blueprint->getX(),
                                                                         squad_focus->getAvgY() + blueprint->getY());
                    list->setSquad(squad_focus);
				}
				else
				{
					if (move_squad && put_squad)
					{
						Coordinates c;
						c.x = mouseX;
						c.y = mouseY;
						squad_focus->moveTo(c);

						put_squad = !put_squad;
						move_squad = !move_squad;
					}
				}
				break;
			default:
				break;
		}
	}
	else
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				for (i = 0; i < Game::getGlobalGame()->getEditingArmy()->nUnits(); i++)
				{
					if (Game::getGlobalGame()->getEditingArmy()->getUnitByID(i)->hover(blueprint->getX(), blueprint->getY()))
					{
						if (squad_focus == Game::getGlobalGame()->getEditingArmy()->getUnitByID(i))
						{
							squad_focus = nullptr;
							list->setVisible(false);
						}
						else
						{
							squad_focus = Game::getGlobalGame()->getEditingArmy()->getUnitByID(i);
							list->setVisible(true);
							if (move_squad)
							{
								put_squad = true;
							}
						}
						list->setSquad(squad_focus);
						break;
					}
				}
				break;
			default:
				break;
		}
	}
}

void Unit_Setup::Logic()
{
	//verifica unit selecionada
	//verifica se um componente foi modificado
	updateGuiElements();
	//faz as naves se moverem
	Game::getGlobalGame()->getEditingArmy()->update();
}

void Unit_Setup::Render()
{
//	int scrWidth = Game::getGlobalGame()->getWidth();
//	int scrHeight = Game::getGlobalGame()->getHeight();
	int mouseX, mouseY;
	Game::getGlobalGame()->setBackgroundColor(255, 0, 0);
	SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();
	imgBackground->DrawImage(renderer);
	drawGuiElements();
	list->draw();
	if (squad_focus)
	{
		squad_selec->DrawImage(
		        squad_focus->getAvgX() + blueprint->getX() - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameWidth() / 2,
		        squad_focus->getAvgY() + blueprint->getY()
		                - Game::getGlobalGame()->getResourceMNGR()->GetImage("squadfocus-bg")->getFrameHeight() / 2, renderer);
	}
	// TODO: Fix coordinates to draw on screen
	Game::getGlobalGame()->getEditingArmy()->render();
//	Game::getGlobalGame()->getEditingArmy()->render(-blueprint->getX(), -blueprint->getY());
	if (put_squad)
	{
		SDL_GetMouseState(&mouseX, &mouseY);
		Game::getGlobalGame()->getResourceMNGR()->GetImage("human-ships")->DrawImage(mouseX, mouseY, squad_type, renderer);
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
