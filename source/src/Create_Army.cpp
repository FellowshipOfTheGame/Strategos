/*
 *  Menu.cpp
 *  Strategos
 *
 *
 */

#include "Global.h"
#include "Game.h"
#include "Create_Army.h"
#include "Image.h"

Create_Army::Create_Army(STATE previous) :
		StateMachine(previous, CREATE_ARMY, CREATE_ARMY)
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();

	Resource *resource = Game::getGlobalGame()->getResourceMNGR();
	resource->AddImage("assets/ui.gfx", "bg_create_army");

	resource->AddImage("assets/ui.gfx", "menu-bt");
	resource->AddImage("assets/ui.gfx", "unit_setup-bt");
	resource->AddImage("assets/ui.gfx", "back-bt");
	resource->AddImage("assets/ui.gfx", "load_army-bt");
	resource->AddImage("assets/ui.gfx", "combo-cmb");


	resource->AddFont("assets/fonts.gfx", "general-gui");
	Font *fntGui = resource->GetFont("general-gui");

	//imagens de fundo
	imgBackground = resource->GetImage("bg_create_army");

	// Botoes
	btn_Next = new Button(520, 660, resource->GetImage("unit_setup-bt"));
	addGuiElement(btn_Next);

	btn_Back = new Button(130, 660, resource->GetImage("back-bt"));
	addGuiElement(btn_Back);

	btn_Load = new Button(scrWidth * 0.5, scrHeight * 0.3, resource->GetImage("load_army-bt"));
	addGuiElement(btn_Load);

	// Labels
	lbl_Title = new Label("Create Army", fntGui, ColorRGB8::Green, ColorRGB8::Black, "LB02");
	lbl_Title->setPosition(scrWidth * 0.45, scrHeight * 0.05);
	addGuiElement(lbl_Title);

	lbl_ArmyName = new Label("Army Name", fntGui, ColorRGB8::Green, ColorRGB8::Black, "LB02");
	lbl_ArmyName->setPosition(360, 120);
	addGuiElement(lbl_ArmyName);

	// Txt Field
	txt_ArmyName = new TextField(510, 120, 190, 28, resource->GetImage("textfield-text"), ColorRGB8::Green, "TF02");
	txt_ArmyName->setFont(fntGui);
	txt_ArmyName->setText("");
	addGuiElement(txt_ArmyName);

	// Tipo do exercito
	cmb_species = new ComboBox(scrWidth * 0.2, scrHeight * 0.6, resource->GetImage("combo-cmb"), ColorRGB8::Green, ColorRGB8::Black);

    cmb_species->setFont(fntGui);
    for (int i = 0; i < Game::getGlobalGame()->getNDictionary(); ++i)
    {
        dct = Game::getGlobalGame()->getDictionary(i);
        cmb_species->addText(dct->title);
    }
	addGuiElement(cmb_species);

    // Default: Dict 0
	dct = Game::getGlobalGame()->getDictionary(0);
	lbl_Descr = new Label("Description", resource->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::Black, "LB02");
	lbl_Descr->setPosition(100, scrHeight * 0.7);
	lbl_Descr->setText(dct->description);
	addGuiElement(lbl_Descr);

	// adicionando box com as imagens das naves
	bx1 = new ImageBox(scrWidth * 0.3, scrHeight * 0.8, 128, 100, dct->getInfoFor(0)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx1);
	bx2 = new ImageBox(scrWidth * 0.4, scrHeight * 0.8, 128, 100, dct->getInfoFor(1)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx2);
	bx3 = new ImageBox(scrWidth * 0.5, scrHeight * 0.8, 128, 100, dct->getInfoFor(2)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx3);
	bx4 = new ImageBox(scrWidth * 0.6, scrHeight * 0.8, 128, 100, dct->getInfoFor(3)->gfx_sfx.shipIdle, 0, nullptr);
	addGuiElement(bx4);

	cmb_armys = new ComboBox(scrWidth * 0.2, scrHeight * 0.3, resource->GetImage("combo-cmb"), ColorRGB8::Green, ColorRGB8::Black);
	cmb_armys->setFont(fntGui);
	addGuiElement(cmb_armys);
	std::vector<std::string> datFiles;
	Resource::getListOfFiles(datFiles, "assets/saves/", ".dat");

	st_box = new StatusBox(0, 0, nullptr, nullptr, "Statusbox");
	st_box->update(0, 0, dct->getInfoFor(0));
	st_box->setVisible(false);
	addGuiElement(st_box);

	for (unsigned int i = 0; i < datFiles.size(); ++i)
	{
		std::string& aux = datFiles[i];
		aux = aux.substr( 0, aux.find_last_of('.') );
		aux = aux.substr( aux.find_last_of("\\/")+1 );
		Army *armytemp = Army::loadArmy(aux.c_str());
		if (armytemp)
			cmb_armys->addText(aux.c_str());
		delete armytemp;
	}

	Game::getGlobalGame()->setEditingArmy(nullptr);
}

Create_Army::~Create_Army()
{
	delete btn_Next;
	delete btn_Back;
	delete btn_Load;

	delete txt_ArmyName;

	delete lbl_ArmyName;
	delete lbl_Title;

	delete bx1;
	delete bx2;
	delete bx3;
	delete bx4;

	delete cmb_species;
	delete cmb_armys;

	delete st_box;
}

void Create_Army::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{
	if (element == btn_Back)
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
	else if (element == btn_Next)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				if (Game::getGlobalGame()->getEditingArmy() == nullptr)
				{
					if (txt_ArmyName->getText() != "")
					{
						if (Game::getGlobalGame()->getDictionary(cmb_species->getText()) != nullptr)
						{
							if (Game::getGlobalGame()->getEditingArmy()==nullptr)
							{
								Game::getGlobalGame()->setEditingArmy(
							        new Army(txt_ArmyName->getText(), Game::getGlobalGame()->getDictionary(cmb_species->getText())));
							}
							else
							{
								Game::getGlobalGame()->getEditingArmy()->setArmyName(txt_ArmyName->getText());
								Game::getGlobalGame()->getEditingArmy()->setDictionary(Game::getGlobalGame()->getDictionary(cmb_species->getText()));
							}

							setNext(UNIT_SETUP);
						}
					}
				}
				else
					setNext(UNIT_SETUP);

				break;

			default:
				break;
		}
	}
	else if (element == btn_Load)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				if (cmb_armys->getSelectedIndex() != -1)
					txt_ArmyName->setText(cmb_armys->getText());
				Game::getGlobalGame()->setEditingArmy(Army::loadArmy(txt_ArmyName->getText().c_str()));
                cmb_species->setSelected(Game::getGlobalGame()->getEditingArmy()->getDictionary()->title.c_str());

				break;

			default:
				break;
		}
	}
	else if (element == cmb_species)
	{
		switch (action)
		{
			case MOUSE_RELEASED_EVENT:
				dct = Game::getGlobalGame()->getDictionary(cmb_species->getSelectedIndex());
				lbl_Descr->setText(dct->description);
				break;

			default:
				break;
		}
	}
	else if (element == cmb_armys)
    {
        switch (action)
        {
            case MOUSE_RELEASED_EVENT:
                break;

            default:
                break;
        }
    }
}

void Create_Army::Logic()
{
    int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();

    if (bx1->hover())
	{
		st_box->update(scrWidth * 0.2, scrHeight * 0.4, dct->getInfoFor(0));
		st_box->setVisible(true);
	}
	else if (bx2->hover())
	{
		st_box->update(scrWidth * 0.3, scrHeight * 0.4, dct->getInfoFor(1));
		st_box->setVisible(true);
	}
	else if (bx3->hover())
	{
		st_box->update(scrWidth * 0.4, scrHeight * 0.4, dct->getInfoFor(2));
		st_box->setVisible(true);
	}
	else if (bx4->hover())
	{
		st_box->update(scrWidth * 0.5, scrHeight * 0.4, dct->getInfoFor(3));
		st_box->setVisible(true);
	}
	else
    {
		st_box->setVisible(false);
	}

	updateGuiElements();
}

void Create_Army::Render()
{
	int scrWidth = Game::getGlobalGame()->getWidth();
	int scrHeight = Game::getGlobalGame()->getHeight();

	Game::getGlobalGame()->setBackgroundColor(0, 0, 0);
	SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();

	imgBackground->DrawImage(renderer);
	drawGuiElements();
}

void Create_Army::Clean()
{

}
