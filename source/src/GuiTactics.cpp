#include <vector>
#include <math.h>

#include "GuiTactics.h"

#include "Global.h"
#include "Game.h"
#include "Image.h"
#include "Unit.h"
#include "Tactic.h"

using namespace ColorRGB8;
//TODO arrumar posicionamento dos elementos
/****************************** TacticSet ******************************/
TacticSet::TacticSet(int x, int y, std::string GID)
{
	this->x = x;
	this->y = y;
	box_container = new Box(x, y, 200, 100, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), nullptr, "BOX" + GID);
	cmb_choiser = new ComboBox(x + 2, y + 2, 200, 20, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black,
	        ColorRGB8::White, "COMBO" + GID);
	cmb_choiser->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	this->GID = GID;

	cmb_choiser->addText("Attack Nearest");
	cmb_choiser->addText("Attack Weakest");
	cmb_choiser->addText("Collaborative Attack");
	cmb_choiser->addText("Collaborative Defense");
	cmb_choiser->addText("Kamikase");
	cmb_choiser->addText("Retreata");
	cmb_choiser->addText("Random Movement");

	guiTactic = new TacticAN(x, 5 + cmb_choiser->getY() + cmb_choiser->getHeight(), GID);

	tg1 = new TriggerSet(x + 2, 5 + guiTactic->getY() + guiTactic->getHeight());
	tg2 = new TriggerSet(x + 2, 5 + tg1->getY() + tg1->getHeight());

	cmb_logic = new ComboBox(x + 2, 5 + tg2->getY() + tg2->getHeight(), 200, 20, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"),
	        ColorRGB8::Black, ColorRGB8::White, "COMBO" + GID);
	cmb_logic->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	cmb_logic->addText("AND");
	cmb_logic->addText("OR");
	this->width = 200;
	this->height = (cmb_logic->getY() + cmb_logic->getHeight()) - cmb_choiser->getY();
}

TacticSet::~TacticSet()
{
	delete box_container;
	delete cmb_choiser;
	delete tg1;
	delete tg2;
	delete cmb_logic;
}

void TacticSet::draw()
{
	box_container->draw();
	cmb_logic->draw();
	tg2->draw();
	tg1->draw();
	guiTactic->draw();
	cmb_choiser->draw();
}

void TacticSet::update()
{


}
int TacticSet::getType()
{
	return tactic->getType();
}

Tactic* TacticSet::getTactic()
{

	Tactic *temp;
	TacticInfo info(tactic->getInfo().allyUnit);
	switch (cmb_choiser->getSelectedIndex())
	{
		case 0:
			temp = Tactic::copy(tactic);//new AttackNearestEnemy(tactic);
			break;
		case 1:
			temp = new AttackWeakestEnemy(info, tactic->getTacticTrigger());
			break;
		case 2:
			temp = new AttackCollab(info, tactic->getTacticTrigger());
			break;
		case 3:
			temp = new DefenseCollab(info, tactic->getTacticTrigger());
			break;
		case 4:
			temp = new Kamikase(info, tactic->getTacticTrigger());
			break;
		case 5:
			temp = new Retreat(info, tactic->getTacticTrigger());
			break;
		case 6:
			temp = new MoveRandomly(info, tactic->getTacticTrigger());
			break;
		default:
			temp = nullptr;
			break;
	}
	return temp;
}
void TacticSet::setTactic(Tactic *t, int id)
{
	unit_id = id;
	if (dynamic_cast<AttackNearestEnemy*>(t))
	{
		cmb_choiser->setSelectedIndex(0);
		convert_AN();
	}
	else if (dynamic_cast<AttackWeakestEnemy*>(t))
	{
		cmb_choiser->setSelectedIndex(1);
		convert_AW();
	}
	else if (dynamic_cast<AttackCollab*>(t))
	{
		cmb_choiser->setSelectedIndex(2);
		convert_CA(id);
	}
	else if (dynamic_cast<DefenseCollab*>(t))
	{
		cmb_choiser->setSelectedIndex(3);
		convert_CD(id);
	}
	else if (dynamic_cast<Kamikase*>(t))
	{
		cmb_choiser->setSelectedIndex(4);
		convert_KM();
	}
	else if (dynamic_cast<Retreat*>(t))
	{
		cmb_choiser->setSelectedIndex(4);
		convert_RT(id);
	}
	else if (dynamic_cast<MoveRandomly*>(t))
	{
		cmb_choiser->setSelectedIndex(5);
		convert_RM();
	}
	else
	{
		printf("erro\n");
	}
	guiTactic->setPosition(x+2, 5+cmb_choiser->getY() + cmb_choiser->getHeight());
	tactic = t;
}
INPUT_EVENT TacticSet::input(SDL_Event &event)
{
	INPUT_EVENT e = NO_EVENT;
	if (cmb_choiser->hover())
	{
		e = cmb_choiser->input(event);
		if (cmb_choiser->getSelectedIndex() != tactic->getType())
		{
			printf ("cliquei na opcao %d", cmb_choiser->getSelectedIndex());
			Tactic *temp = getTactic();
			delete tactic;
			setTactic(temp, unit_id);
			tg1->setPosition(x + 2, 5 + guiTactic->getY() + guiTactic->getHeight());
			tg2->setPosition(x + 2, 5 + tg1->getY() + tg1->getHeight());

			cmb_logic->setPosition(x + 2, 5 + tg2->getY() + tg2->getHeight());
			this->height = (cmb_logic->getY() + cmb_logic->getHeight()) - cmb_choiser->getY();
			printf ("tactic type: %d\n",tactic->getType());
		}
	}
	else if (guiTactic->hover())
	{
		e = guiTactic->input(event);
	}
	else if (tg1->hover())
	{
		e = tg1->input(event);
		tactic->getTacticTrigger().setTriggerA(tg1->getTrigger());
	}
	else if (tg2->hover())
	{
		e = tg2->input(event);
		tactic->getTacticTrigger().setTriggerB(tg2->getTrigger());
	}
	else if (cmb_logic->hover())
	{
		e = cmb_logic->input(event);
		tactic->getTacticTrigger().setLogicOperator(cmb_logic->getSelectedIndex());
	}
	return e;
}

void TacticSet::convert_AN()
{
	if (dynamic_cast<TacticAN*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic= new TacticAN(x, y, GID);
	}
}
void TacticSet::convert_AW()
{
	if (dynamic_cast<TacticAW*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic= new TacticAW(x, y, GID);
	}
}
void TacticSet::convert_CA(int id)
{
	if (dynamic_cast<TacticCA*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic = new TacticCA(x, y, id, GID);
	}
}
void TacticSet::convert_CD(int id)
{
	if (dynamic_cast<TacticCD*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic = new TacticCD(x, y, id, GID);
	}

}
void TacticSet::convert_KM()
{
	if (dynamic_cast<TacticKM*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic = new TacticKM(x, y, GID);
	}
}
void TacticSet::convert_RT(int id)
{
	if (dynamic_cast<TacticRT*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic = new TacticRT(x, y, id, GID);
	}
}
void TacticSet::convert_RM()
{
	if (dynamic_cast<TacticRM*>(this->guiTactic) == nullptr)
	{
		delete guiTactic;
		guiTactic = new TacticRM(x, y, GID);
	}
}
/*GuiTactic*/
GuiTactic::GuiTactic(int x, int y, std::string GID)
{
	this->x = x;
	this->y = y;
	this->GID = GID;
	this->height = 20;
}

GuiTactic::~GuiTactic()
{
}

void GuiTactic::draw()
{
}
void GuiTactic::update(int x, int y, DictKey *key)
{
}

INPUT_EVENT GuiTactic::input(SDL_Event &event)
{
	return NO_EVENT;
}

void GuiTactic::setDistance(int i)
{
}
int GuiTactic::getDistance()
{
	return 0;
}

/****************************** TacticAN ******************************/
TacticAN::TacticAN(int x, int y, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->height=0;
}

TacticAN::~TacticAN()
{
}

void TacticAN::draw()
{
}
void TacticAN::update(int x, int y, DictKey *key)
{
}

INPUT_EVENT TacticAN::input(SDL_Event &event)
{

	return NO_EVENT;
}

/****************************** TacticAW ******************************/
TacticAW::TacticAW(int x, int y, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->height=0;
}

TacticAW::~TacticAW()
{
}

void TacticAW::draw()
{

}
void TacticAW::update(int x, int y, DictKey *key)
{

}

INPUT_EVENT TacticAW::input(SDL_Event &event)
{

	return NO_EVENT;
}

/****************************** TacticCA ******************************/
TacticCA::TacticCA(int x, int y, int id, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	cmb_partner = new ComboBox(x, y, 50, 30, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black, ColorRGB8::White, "COMBO" + GID);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[5];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (unsigned int i = 0; i < unt.size(); i++)
	{
		if (id != unt[i]->getID())
		{
            sprintf(str, "%d", unt[i]->getID());
            cmb_partner->addText(str);
		}
	}
	this->height = cmb_partner->getHeight();
}

TacticCA::~TacticCA()
{
	delete cmb_partner;
}

void TacticCA::draw()
{
	cmb_partner->setPosition(this->x,this->y);
	cmb_partner->draw();
}

void TacticCA::update(int x, int y, DictKey *key)
{
}

int TacticCA::getPartner()
{
	return cmb_partner->getSelectedIndex();
}

INPUT_EVENT TacticCA::input(SDL_Event &event)
{
	if (cmb_partner->hover())
		return cmb_partner->input(event);
	return NO_EVENT;
}
/****************************** TacticCD ******************************/
TacticCD::TacticCD(int x, int y, int id, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	cmb_partner = new ComboBox(x , y, 50, 30, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black, ColorRGB8::White, "COMBO" + GID);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[5];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (unsigned int i = 0; i < unt.size(); i++)
	{
		if (id != unt[i]->getID())
        {
            sprintf(str, "%d", unt[i]->getID());
            cmb_partner->addText(str);
        }
	}
	this->height = cmb_partner->getHeight();
}

TacticCD::~TacticCD()
{
	delete cmb_partner;
}

void TacticCD::draw()
{

	cmb_partner->setPosition(this->x,this->y);
	cmb_partner->draw();
}
void TacticCD::update(int x, int y, DictKey *key)
{
}
int TacticCD::getPartner()
{
	return cmb_partner->getSelectedIndex();
}
INPUT_EVENT TacticCD::input(SDL_Event &event)
{
	if (cmb_partner->hover())
		return cmb_partner->input(event);
	return NO_EVENT;
}
/****************************** TacticKM ******************************/
TacticKM::TacticKM(int x, int y, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->height=0;
}

TacticKM::~TacticKM()
{
}

void TacticKM::draw()
{
}
void TacticKM::update(int x, int y, DictKey *key)
{
}

INPUT_EVENT TacticKM::input(SDL_Event &event)
{
	return NO_EVENT;
}
/****************************** TacticRT ******************************/
TacticRT::TacticRT(int x, int y, int id, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	cmb_partner = new ComboBox(x , y , 50, 30, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black, ColorRGB8::White, "COMBO" + GID);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[5];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (unsigned int i = 0; i < unt.size(); i++)
	{
		if (id != unt[i]->getID())
        {
            sprintf(str, "%d", unt[i]->getID());
            cmb_partner->addText(str);
        }
	}
	this->height = cmb_partner->getHeight();
}

TacticRT::~TacticRT()
{
	delete cmb_partner;
}

void TacticRT::draw()
{
	cmb_partner->setPosition(this->x,this->y);
	cmb_partner->draw();
}
void TacticRT::update(int x, int y, DictKey *key)
{
}
int TacticRT::getPartner()
{
	return cmb_partner->getSelectedIndex();
}
INPUT_EVENT TacticRT::input(SDL_Event &event)
{
	if (cmb_partner->hover())
		return cmb_partner->input(event);
	return NO_EVENT;
}
/****************************** TacticRM ******************************/
TacticRM::TacticRM(int x, int y, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->height=0;
}

TacticRM::~TacticRM()
{
}

void TacticRM::draw()
{
}
void TacticRM::update(int x, int y, DictKey *key)
{
}

INPUT_EVENT TacticRM::input(SDL_Event &event)
{
	return NO_EVENT;
}
/*************************************************************/
void TacticList::initialize()
{
	Resource *resource = Game::getGlobalGame()->getResourceMNGR();
	btn_up = new Button(this->x, this->y, 200, 20, resource->GetImage("menu-bt"));
	btn_up->setText(resource->GetFont("jostix-14"), "A", ColorRGB8::White, ColorRGB8::White);

	btn_down = new Button(this->x, 5 + this->y + 200, 200, 20, resource->GetImage("menu-bt"));
	btn_down->setText(resource->GetFont("jostix-14"), "V", ColorRGB8::White, ColorRGB8::White);

	btn_plus = new Button(this->x, 5 + btn_down->getY() + btn_down->getHeight(), 100, 20, resource->GetImage("menu-bt"));
	btn_plus->setText(resource->GetFont("jostix-14"), "+", ColorRGB8::White, ColorRGB8::White);

	btn_minus = new Button(this->x + 101, 5 + btn_down->getY() + btn_down->getHeight(), 100, 20, resource->GetImage("menu-bt"));
	btn_minus->setText(resource->GetFont("jostix-14"), "-", ColorRGB8::White, ColorRGB8::White);

	btn_show = new Button(this->x - 40, this->y, 40, 100, resource->GetImage("menu-bt"));
	btn_show->setText(resource->GetFont("jostix-14"), "Hidden", ColorRGB8::White, ColorRGB8::White);
	top = 0;
	selected = nullptr;
	tct_set = new TacticSet(this->x, 5 + btn_minus->getY() + btn_minus->getHeight(), "TCT");
	this->height = btn_minus->getY() + btn_minus->getHeight();
	squad = nullptr;
}
TacticList::TacticList(Unit *squad, int x, int y)
{
	this->x = x;
	this->y = y;
	this->width = 200;
	this->height = 0;
	initialize();
	Resource *resource = Game::getGlobalGame()->getResourceMNGR();
	for (unsigned int i = 0; i < squad->getTacticSize(); i++)
	{
		lista.push_back(new ItemList("default", resource->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::White, "LB02"));
	}
}

TacticList::TacticList(int x, int y)
{
	this->x = x;
	this->y = y;
	this->width = 200;
	this->height = 0;
	initialize();
}

TacticList::~TacticList()
{
	delete btn_up;
	delete btn_down;
	delete btn_plus;
	delete btn_minus;
	delete btn_show;
	delete tct_set;

	std::vector<ItemList*>::iterator iter = lista.begin();
	while (iter != lista.end())
	{
		delete *iter;
		iter = lista.erase(iter);
	}
}

void TacticList::draw()
{
	int max = (lista.size() < top + 5) ? lista.size() : top + 5;
	if (this->shown)
	{
		btn_up->draw();
		btn_down->draw();
		btn_plus->draw();
		btn_minus->draw();
		if (top < lista.size())
		{
			if (this->selected)
				selected->setType(tct_set->getType());
			for (int i = top; i < max; i++)
			{
				lista[i]->setPosition(this->x, this->y + 25 + 22 * (i - top));
				lista[i]->draw();
			}
		}
		if (this->selected)
			tct_set->draw();
	}
	btn_show->draw();
}

void TacticList::setSquad(Unit *squad)
{
	selected = nullptr;
	if (squad != this->squad)
	{
		this->squad = squad;
		if (this->squad == nullptr)
			return;

		std::vector<ItemList*>::iterator iter = lista.begin();
		printf("removendo taticas antigas\n");
		//remove elementos da antiga squad
		iter = lista.begin();
		while (iter != lista.end())
		{
			delete (*iter);
			iter = lista.erase(iter);
		}
		printf ("recriando lista - begin\n");
		//coloca Top no topo
		top = 0;
		//adiciona taticas da nova squad na lista

		unsigned int i = 0;
		int tp = -1;
		while (i < squad->getTacticSize())
		{
			if (dynamic_cast<AttackNearestEnemy*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 0;
			}
			else if (dynamic_cast<AttackWeakestEnemy*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 1;
			}
			else if (dynamic_cast<AttackCollab*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 2;
			}
			else if (dynamic_cast<DefenseCollab*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 3;
			}
			else if (dynamic_cast<Kamikase*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 4;
			}
			else if (dynamic_cast<Retreat*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 5;
			}
			else if (dynamic_cast<MoveRandomly*>(squad->getTacticAt(i)) != nullptr)
			{
				tp = 6;
			}
			printf("tatica tipo %d\n", tp);
			lista.push_back(new ItemList(tp));
			i++;
		}

		printf ("recriando lista - end\n");
	}
}

void TacticList::update()
{
	if (this->squad == nullptr)
		return;
	if (this->selected == nullptr)
		return;
	printf ("update - begin\n");
	//encontra o interador da opcao selecionada
	std::vector<ItemList*>::iterator iter = lista.begin();
	while (iter != lista.end())
	{
		if ((*iter) == selected)
			break;
		iter++;
	}
	//encontra o indice desta opcao
	int index = distance(lista.begin(), iter);

	//adiciona taticas da nova squad na lista
	if (squad)
	{
		int tp = -1;

		if (dynamic_cast<AttackNearestEnemy*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 0;
		}
		else if (dynamic_cast<AttackWeakestEnemy*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 1;
		}
		else if (dynamic_cast<AttackCollab*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 2;
		}
		else if (dynamic_cast<DefenseCollab*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 3;
		}
		else if (dynamic_cast<Kamikase*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 4;
		}
		else if (dynamic_cast<Retreat*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 5;
		}
		else if (dynamic_cast<MoveRandomly*>(squad->getTacticAt(index)) != nullptr)
		{
			tp = 6;
		}
		selected->setType(tp);
	}
	printf ("update - end\n");
}

bool TacticList::hover()
{
	bool hover = btn_up->hover() | btn_down->hover() | btn_plus->hover() | btn_minus->hover() | btn_show->hover() | tct_set->hover();
	std::vector<ItemList*>::iterator iter = lista.begin();
	while (iter != lista.end())
	{
		hover = hover | (*iter)->hover();
	}
	return hover;
}

INPUT_EVENT TacticList::input(SDL_Event &event)
{
	if ((this->selected) && (tct_set->hover()))
	{
		INPUT_EVENT e = tct_set->input(event);
		if (e != NO_EVENT)
		{
			if ((selected != nullptr) && (squad->getTacticSize() > 0))
			{
				int i = 0;
				std::vector<ItemList*>::iterator iter = lista.begin();
				while (iter != lista.end())
				{
					if ((*iter) == selected)
					{
						squad->setTacticAt(tct_set->getTactic(), i);
						break;
					}
					++i;
					++iter;
				}
			}
		}
		return e;
	}
	else if (btn_up->hover())
	{
		switch (btn_up->input(event))
		{
			case MOUSE_RELEASED_EVENT:
				if (top > 0)
					top--;
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
	}
	else if (btn_down->hover())
	{
		switch (btn_down->input(event))
		{
			case MOUSE_RELEASED_EVENT:
				if (top + 5 < lista.size())
					top++;
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
	}
	else if (btn_plus->hover())
	{
		TacticInfo info(0);
		switch (btn_plus->input(event))
		{

			case MOUSE_RELEASED_EVENT:
				lista.push_back(new ItemList());
				squad->addTactic(new AttackNearestEnemy(info, TacticTrigger(new Trigger_Always(), new Trigger_Always(), 0)));
				//selected = *(lista.end());
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
	}
	else if (btn_minus->hover())
	{
		switch (btn_minus->input(event))
		{
			case MOUSE_RELEASED_EVENT:
				if ((selected != nullptr) && (squad->getTacticSize() > 0))
				{
					int i = 0;
					std::vector<ItemList*>::iterator iter = lista.begin();
					while (iter != lista.end())
					{
						if ((*iter) == selected)
						{
							iter = lista.erase(lista.begin() + i);

							break;
						}
						i++;
					}
					squad->removeTactic(i);
					delete selected;
					selected = nullptr;
				}
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
	}
	else if (btn_show->hover())
	{
		switch (btn_show->input(event))
		{
			case MOUSE_RELEASED_EVENT:
				this->shown= !this->shown;
				return  MOUSE_RELEASED_EVENT;
			default:
				break;
		}
	}
	else
	{
		for (unsigned int i = 0; i < lista.size(); i++)
		{
			if (lista[i]->hover())
			{
				switch (event.type)
				{
					case SDL_MOUSEBUTTONUP:
						if (selected == nullptr)
						{
							this->height += tct_set->getHeight();
						}
						if (selected == lista[i])
						{
							selected->setSelected(false);
							selected = nullptr;
							this->height -= tct_set->getHeight();
						}
						else
						{
							selected->setSelected(false);
							selected = lista[i];
							selected->setSelected(true);
							tct_set->setTactic(squad->getTacticAt(i), squad->getID());
						}
						return MOUSE_RELEASED_EVENT;
				}
			}
		}
	}
	return NO_EVENT;
}
/*************************************************************/
void ItemList::setType(int i)
{
	this->type = i;
	initialize();
}
void ItemList::initialize()
{
	switch (this->type)
	{
		case 0:
			this->setText("ATTACK_NEAREST_ENEMY");
			break;
		case 1:
			this->setText("ATTACK_WEAKEST_ENEMY");
			break;
		case 2:
			this->setText("ATTACK_COLLAB");
			break;
		case 3:
			this->setText("DEFENSE_COLAB");
			break;
		case 4:
			this->setText("KAMIKASE");
			break;
		case 5:
			this->setText("RETREAT");
			break;
		case 6:
			this->setText("MOVE_RANDOM");
			break;
	}
	this->height = 20;
	this->width = 100;
}

ItemList::ItemList(std::string text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID) :
		Label(text, font, color, shadow, GID)
{
	type = 0;
	initialize();
}
ItemList::ItemList(int tp, std::string text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID) :
		Label(text, font, color, shadow, GID)
{
	this->type = tp;
	initialize();
}
ItemList::ItemList(int tp) :
		Label("ATTACK_NEAREST_ENEMY", Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::White, "LB02")
{
	this->type = tp;
	initialize();

}

ItemList::ItemList() :
		Label("ATTACK_NEAREST_ENEMY", Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::White, "LB02")
{
	this->type = 0;
	initialize();
}
ItemList::~ItemList()
{
}
void ItemList::draw()
{
	this->Label::draw();
}

void ItemList::setSelected(bool selected)
{
    SDL_Color cor;
    cor = ColorRGB8::Green;
    if (selected)
    	cor = ColorRGB8::Red;
	this->setColor(cor);
}

INPUT_EVENT ItemList::input(SDL_Event &event)
{
	if (hover())
	{
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				return MOUSE_PRESSED_EVENT;

			case SDL_MOUSEBUTTONUP:
				return MOUSE_RELEASED_EVENT;
		}
	}
	return NO_EVENT;
}
/*************************************************************/
TriggerSet::TriggerSet(int x, int y)
{
	this->x = x;
	this->y = y;
	this->width = 100;
	cmb_trigger = new ComboBox(x, y, 200, 20, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black, ColorRGB8::White,
	        "COMBO" + GID);
	cmb_trigger->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	cmb_trigger->addText("ALWAYS");
	cmb_trigger->addText("LIFE");
	cmb_trigger->addText("SHIELD");

	cmb_operation = new ComboBox(x, cmb_trigger->getY() + cmb_trigger->getHeight(), 200, 20,
	        Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::Black, ColorRGB8::White, "COMBO" + GID);
	cmb_operation->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	cmb_operation->addText("    =   ");
	cmb_operation->addText("   <=   ");
	cmb_operation->addText("   >=   ");

	value = new TextField(x, cmb_operation->getY() + cmb_operation->getHeight(), 190, 20,
	        Game::getGlobalGame()->getResourceMNGR()->GetImage("textfield-text"), ColorRGB8::White, "TF02");
	value->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	value->setText("100");
	this->height = (value->getY() + value->getHeight()) - cmb_trigger->getY();
}
TriggerSet::~TriggerSet()
{
	delete cmb_trigger;
	delete cmb_operation;
	delete value;
}
Trigger* TriggerSet::getTrigger()
{
	Trigger *tg = 0;
	switch (cmb_trigger->getSelectedIndex())
	{
		case 0:
			tg = new Trigger_Always();
			break;
		case 1:
			tg = new Trigger_Life(atoi(value->getText().c_str()), cmb_operation->getSelectedIndex());
			break;
		case 2:
			tg = new Trigger_Life(atoi(value->getText().c_str()), cmb_operation->getSelectedIndex());
			break;
	}
	return tg;
}
void TriggerSet::draw()
{
	cmb_trigger->setPosition(this->x, this->y);
	cmb_operation->setPosition(this->x, cmb_trigger->getY()+cmb_trigger->getHeight());
	value->setPosition(this->x,cmb_operation->getY()+cmb_operation->getHeight());


	value->draw();
	cmb_operation->draw();
	cmb_trigger->draw();
}
INPUT_EVENT TriggerSet::input(SDL_Event &event)
{
	INPUT_EVENT e = NO_EVENT;
	if (cmb_trigger->hover())
	{
		e = cmb_trigger->input(event);
	}
	else if (cmb_operation->hover())
	{
		e = cmb_operation->input(event);
	}
	else
	{
		e = value->input(event);
	}
	return e;
}
