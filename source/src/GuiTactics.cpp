#include <vector>
#include <math.h>

#include "GuiTactics.h"

#include "Global.h"
#include "Game.h"
#include "Image.h"
#include "Unit.h"
#include "Tactic.h"

using namespace ColorRGB8;

/****************************** TacticSet ******************************/
TacticSet::TacticSet(int x, int y, std::string GID)
{
	this->x = x;
	this->y = y;
	box_container = new Box(x, y, 200, 100, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), nullptr);
	cmb_choiser = new ComboBox(x + 2, y + 2, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_choiser->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	this->GID = GID;

	cmb_choiser->addText("Attack Nearest");
	cmb_choiser->addText("Attack Weakest");
	cmb_choiser->addText("Collaborative Attack");
	cmb_choiser->addText("Collaborative Defense");
	cmb_choiser->addText("Kamikase");
	cmb_choiser->addText("Retreat");
	cmb_choiser->addText("Random Movement");

	guiTactic = new TacticAN(x, 5 + cmb_choiser->getY() + cmb_choiser->getHeight(), GID);

	tg1 = new TriggerSet(x + 2, 5 + guiTactic->getY() + guiTactic->getHeight()+22);
	tg2 = new TriggerSet(x + 2, 5 + tg1->getY() + tg1->getHeight());

	cmb_logic = new ComboBox(x + 2, 5 + tg2->getY() + tg2->getHeight(), Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
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
	return guiTactic->getType();
}

Tactic* TacticSet::getTactic()
{
	Tactic *temp;
	TacticInfo info(Game::getGlobalGame()->getEditingArmy()->getUnitAtIndex(guiTactic->getPartner()));
    TacticTrigger tt(tg1->getTrigger(), tg2->getTrigger(), cmb_logic->getSelectedIndex());

	switch (cmb_choiser->getSelectedIndex())
	{
		case 0:
			temp = new AttackNearestEnemy(info, tt);
			break;
		case 1:
			temp = new AttackWeakestEnemy(info, tt);
			break;
		case 2:
			temp = new AttackCollab(info, tt);
			break;
		case 3:
			temp = new DefenseCollab(info, tt);
			break;
		case 4:
			temp = new Kamikase(info, tt);
			break;
		case 5:
			temp = new Retreat(info, tt);
			break;
		case 6:
			temp = new MoveRandomly(info, tt);
			break;
		default:
			temp = nullptr;
			break;
	}

	return temp;
}

void TacticSet::setUItriggers(const TacticTrigger& trigger)
{
    cmb_logic->setSelectedIndex( trigger.getLogicOperator() );

    tg1->setType(trigger.getTriggerA()->getType());
    tg1->setValue(trigger.getTriggerA()->getValue());
    tg1->setOperation(trigger.getTriggerA()->getRelationalOperator());

    tg2->setType(trigger.getTriggerB()->getType());
    tg2->setValue(trigger.getTriggerB()->getValue());
    tg2->setOperation(trigger.getTriggerB()->getRelationalOperator());
}

void TacticSet::setTactic(Tactic *tactic, int id)
{
    cmb_choiser->setSelectedIndex(tactic->getType());

    delete guiTactic;

    switch (tactic->getType())
    {
	    case TACTIC_ATTACK_NEAREST_ENEMY:
            guiTactic = new TacticAN(x,y, GID);
        break;

	    case TACTIC_ATTACK_WEAKEST_ENEMY:
            guiTactic = new TacticAW(x,y, GID);
        break;

	    case TACTIC_ATTACK_COLLAB:
            guiTactic = new TacticCA(x,y, id, tactic->getInfo().allyUnit->getID(), GID);
        break;

	    case TACTIC_DEFENSE_COLLAB:
            guiTactic = new TacticCD(x,y, id, tactic->getInfo().allyUnit->getID(), GID);
        break;

        case TACTIC_KAMIKASE:
            guiTactic = new TacticKM(x,y, GID);
        break;

        case TACTIC_RETREAT:
            guiTactic = new TacticRT(x,y, id, tactic->getInfo().allyUnit->getID(), GID);
        break;

        case TACTIC_MOVE_RANDOM:
            guiTactic = new TacticRM(x,y, GID);
        break;

        default:
            printf("ERRO\n");
        break;
	}

	setUItriggers(tactic->getTacticTrigger());

	guiTactic->setPosition(x+2, 5+cmb_choiser->getY() + cmb_choiser->getHeight());
}

void TacticSet::setTactic(int t, int id)
{
    printf("CONVERTE SAPORRA PARA %d\n", t);
	unit_id = id;

	cmb_choiser->setSelectedIndex(t);

	switch (t)
	{
	    case TACTIC_ATTACK_NEAREST_ENEMY:
            convert_AN();
        break;

	    case TACTIC_ATTACK_WEAKEST_ENEMY:
            convert_AW();
        break;

	    case TACTIC_ATTACK_COLLAB:
            convert_CA(id);
        break;

	    case TACTIC_DEFENSE_COLLAB:
            convert_CD(id);
        break;

        case TACTIC_KAMIKASE:
            convert_KM();
        break;

        case TACTIC_RETREAT:
            convert_RT(id);
        break;

        case TACTIC_MOVE_RANDOM:
            convert_RM();
        break;

        default:
            printf("ERRO\n");
        break;
	}

	guiTactic->setPosition(x+2, 5+cmb_choiser->getY() + cmb_choiser->getHeight());
//	tactic = t;
}

INPUT_EVENT TacticSet::input(SDL_Event &event)
{
	INPUT_EVENT e = NO_EVENT;

	e = cmb_choiser->input(event);
	if (e != NO_EVENT)
	{
		if (cmb_choiser->getSelectedIndex() != guiTactic->getType())
		{
			printf ("cliquei na opcao %d", cmb_choiser->getSelectedIndex());
			setTactic(cmb_choiser->getSelectedIndex(), unit_id);

			this->height = (cmb_logic->getY() + cmb_logic->getHeight()) - cmb_choiser->getY();
			printf ("tactic type: %d\n", guiTactic->getType());
		}

		return e;
	}

    e = guiTactic->input(event);
	if (e != NO_EVENT){
	    return e;
	}

	e = tg1->input(event);
    if (e != NO_EVENT)
	{
	    tg2->setActive(false);
		return e;
	}

    e = tg2->input(event);
	if (e != NO_EVENT)
	{
	    tg1->setActive(false);
		return e;
	}

    e = cmb_logic->input(event);
	if (e != NO_EVENT)
	{
	    printf("cmb_logic %d\n", e);
		return e;
	}

	return NO_EVENT;
}

void TacticSet::convert_AN()
{
    if (guiTactic->getType() != TACTIC_ATTACK_NEAREST_ENEMY){
		delete guiTactic;
		guiTactic = new TacticAN(x, y, GID);
	}
}
void TacticSet::convert_AW()
{
    if (guiTactic->getType() != TACTIC_ATTACK_WEAKEST_ENEMY){
		delete guiTactic;
		guiTactic= new TacticAW(x, y, GID);
	}
}
void TacticSet::convert_CA(int id)
{
    if (guiTactic->getType() != TACTIC_ATTACK_COLLAB){
        GuiTactic *old = guiTactic;
		guiTactic = new TacticCA(x, y, id, guiTactic->getPartner(), GID);
		delete old;
	}
}
void TacticSet::convert_CD(int id)
{
    if (guiTactic->getType() != TACTIC_DEFENSE_COLLAB){
        GuiTactic *old = guiTactic;
		guiTactic = new TacticCD(x, y, id, guiTactic->getPartner(), GID);
		delete old;
	}

}
void TacticSet::convert_KM()
{
    if (guiTactic->getType() != TACTIC_KAMIKASE){
		delete guiTactic;
		guiTactic = new TacticKM(x, y, GID);
	}
}
void TacticSet::convert_RT(int id)
{
    if (guiTactic->getType() != TACTIC_RETREAT){
		GuiTactic *old = guiTactic;
		guiTactic = new TacticRT(x, y, id, guiTactic->getPartner(), GID);
		delete old;
	}
}
void TacticSet::convert_RM()
{
    if (guiTactic->getType() != TACTIC_MOVE_RANDOM){
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

INPUT_EVENT GuiTactic::input(SDL_Event &event)
{
	return NO_EVENT;
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

/****************************** TacticAW ******************************/
TacticAW::TacticAW(int x, int y, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->height=0;
}

TacticAW::~TacticAW()
{
}

/****************************** TacticCA ******************************/
TacticCA::TacticCA(int x, int y, int id, int ally, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	selfUnitID = id;
	cmb_partner = new ComboBox(x, y, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[9];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (unsigned int i = 0; i < unt.size(); i++)
	{
		if (id == unt[i]->getID()){
		    sprintf(str, "<SELF>");
		}else{
            sprintf(str, "%d", unt[i]->getID());
		}

        cmb_partner->addText(str);
	}
	this->height = cmb_partner->getHeight();

	if ( ally >= 0 && ally < unt.size() ){
        cmb_partner->setSelectedIndex(ally-1);
	}
	setPartner(ally);
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

int TacticCA::getPartner()
{
	return cmb_partner->getSelectedIndex();
}

void TacticCA::setPartner( int id )
{
    cmb_partner->setSelectedIndex(id);
}

INPUT_EVENT TacticCA::input(SDL_Event &event)
{
	if (cmb_partner->hover()){
        cmb_partner->input(event);

        if (cmb_partner->getSelectedIndex() == selfUnitID)
            cmb_partner->setSelectedIndex(0);
	}
	return NO_EVENT;
}
/****************************** TacticCD ******************************/
TacticCD::TacticCD(int x, int y, int id, int ally, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	selfUnitID = id;
	cmb_partner = new ComboBox(x , y, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[9];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (size_t i = 0; i < unt.size(); i++)
	{
		if (id == unt[i]->getID()){
		    sprintf(str, "<SELF>");
		}else{
            sprintf(str, "%d", unt[i]->getID());
		}

        cmb_partner->addText(str);
	}
	this->height = cmb_partner->getHeight();

    if ( ally >= 0 && ally < unt.size() ){
        cmb_partner->setSelectedIndex(ally-1);
	}
	setPartner(ally);

	printf("\n\nATTACK COLLAB\n\n");
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
int TacticCD::getPartner()
{
	return cmb_partner->getSelectedIndex();
}

void TacticCD::setPartner( int id )
{
    cmb_partner->setSelectedIndex(id);
}

INPUT_EVENT TacticCD::input(SDL_Event &event)
{
	if (cmb_partner->hover()){
		return cmb_partner->input(event);

		if (cmb_partner->getSelectedIndex() == selfUnitID)
            cmb_partner->setSelectedIndex(0);
	}
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

/****************************** TacticRT ******************************/
TacticRT::TacticRT(int x, int y, int id, int ally, std::string GID) :
		GuiTactic(x, y, GID)
{
	this->x=x;
	this->y=y;
	selfUnitID = id;
	cmb_partner = new ComboBox(x , y, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_partner->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));

	char str[9];
	const std::vector<Unit*>& unt = Game::getGlobalGame()->getEditingArmy()->getUnits();
	for (unsigned int i = 0; i < unt.size(); i++)
	{
		if (id == unt[i]->getID()){
		    sprintf(str, "<SELF>");
		}else{
            sprintf(str, "%d", unt[i]->getID());
		}

        cmb_partner->addText(str);
	}
	this->height = cmb_partner->getHeight();

    if ( ally >= 0 && ally < unt.size() ){
        cmb_partner->setSelectedIndex(ally-1);
	}
	setPartner(ally);
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
int TacticRT::getPartner()
{
	return cmb_partner->getSelectedIndex();
}

void TacticRT::setPartner( int id )
{
    cmb_partner->setSelectedIndex(id);
}

INPUT_EVENT TacticRT::input(SDL_Event &event)
{
	if (cmb_partner->hover()){
		return cmb_partner->input(event);

		if (cmb_partner->getSelectedIndex() == selfUnitID)
            cmb_partner->setSelectedIndex(0);
	}
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

	btn_show = new Button(this->x - 42, this->y, 40, 100, resource->GetImage("menu-bt"));
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
	int max = (lista.size() < top + 5) ? (int)lista.size() : top + 5;
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

    this->squad = squad;
    if (this->squad == nullptr)
        return;

    std::vector<ItemList*>::iterator iter = lista.begin();
    //remove elementos da antiga squad
    iter = lista.begin();
    while (iter != lista.end())
    {
        delete (*iter);
        iter = lista.erase(iter);
    }
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
        lista.push_back(new ItemList(tp));
        i++;
    }
}

void TacticList::update()
{
	if (this->squad == nullptr)
		return;
	if (this->selected == nullptr)
		return;

	//encontra o interador da opcao selecionada
	std::vector<ItemList*>::iterator iter = lista.begin();
	while (iter != lista.end())
	{
		if ((*iter) == selected)
			break;
		iter++;
	}
	//encontra o indice desta opcao
	int index = (int)distance(lista.begin(), iter);

	//adiciona taticas da nova squad na lista
	if (squad)
	{
		selected->setType(squad->getTacticAt(index)->getType());
	}
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
    INPUT_EVENT e = NO_EVENT;

	if ( this->selected )
	{
	    e = tct_set->input(event);
        if (e != NO_EVENT && squad->getTacticSize() > 0 )
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

            return e;
        }
	}

	e = btn_up->input(event);
	if (e != NO_EVENT)
	{
		switch ( e )
		{
			case MOUSE_RELEASED_EVENT:
				if (top > 0)
					top--;
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}

		return e;
	}

	e = btn_down->input(event);
	if ( e != NO_EVENT )
	{
		switch (e)
		{
			case MOUSE_RELEASED_EVENT:
				if (top + 5 < lista.size())
					top++;
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
		return e;
	}

	e = btn_plus->input(event);
	if ( e != NO_EVENT )
	{
		TacticInfo info(0);
		switch (e)
		{
			case MOUSE_RELEASED_EVENT:
				lista.push_back(new ItemList());
				squad->addTactic(new AttackNearestEnemy(info, TacticTrigger(new Trigger_Always(), new Trigger_Always(), 0)));
				//selected = *(lista.end());
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
		return e;
	}

	e = btn_minus->input(event);
	if (e != NO_EVENT)
	{
		switch (e)
		{
			case MOUSE_RELEASED_EVENT:
				if ( (selected != nullptr) && (squad->getTacticSize() > 0) )
				{
					int i = 0;
					std::vector<ItemList*>::iterator iter = lista.begin();
					while (iter != lista.end())
					{
						if ((*iter) == selected)
						{
							iter = lista.erase(iter);
							break;
						}
						++i;
					}

					squad->removeTactic(i);
					delete selected;
					selected = nullptr;
				}
				return MOUSE_RELEASED_EVENT;

			default:
				break;
		}
		return e;
	}

	e = btn_show->input(event);
	if (e != NO_EVENT)
	{
		switch (e)
		{
			case MOUSE_RELEASED_EVENT:
				this->shown= !this->shown;
				return  MOUSE_RELEASED_EVENT;
			default:
				break;
		}

		return e;
	}

	// Checar se clicou em outro item da lista
    for (unsigned int i = 0; i < lista.size(); i++)
    {
        if (lista[i]->hover())
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONUP:
                    if (selected == lista[i])
                    {
                        selected->setSelected(false);
                        selected = nullptr;
                        this->height -= tct_set->getHeight();

                        // Adicionar tamanho do editor de taticas
                        if (selected == nullptr){
                            this->height += tct_set->getHeight();
                        }
                    }
                    else
                    {
                        if (selected){
                            selected->setSelected(false);
                        }

                        selected = lista[i];
                        selected->setSelected(true);

                        tct_set->setTactic(squad->getTacticAt(i), squad->getID());
                    }
                return MOUSE_RELEASED_EVENT;
            }

            return MOUSE_OVER;
        }
    }

	return NO_EVENT;
}
/*************************************************************/
void ItemList::setType(int i)
{
	this->type = i;
	this->initialize();
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
	this->setType(0);
}
ItemList::ItemList(int tp, std::string text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID) :
		Label(text, font, color, shadow, GID)
{
	this->setType(tp);
}
ItemList::ItemList(int tp) :
		Label("ATTACK_NEAREST_ENEMY", Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::White, "LB02")
{
	this->setType(tp);

}

ItemList::ItemList() :
		Label("ATTACK_NEAREST_ENEMY", Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"), ColorRGB8::Green, ColorRGB8::White, "LB02")
{
	this->setType(0);
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
	cmb_trigger = new ComboBox(x, y, Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_trigger->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	cmb_trigger->addText("ALWAYS");
	cmb_trigger->addText("LIFE");

	cmb_operation = new ComboBox(x, cmb_trigger->getY() + cmb_trigger->getHeight(), Game::getGlobalGame()->getResourceMNGR()->GetImage("combo-cmb"), ColorRGB8::White, ColorRGB8::White);
	cmb_operation->setFont(Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14"));
	cmb_operation->addText("   = ");
	cmb_operation->addText("  <= ");
	cmb_operation->addText("  >= ");

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
	}
	return tg;
}

void TriggerSet::setType(int type)
{
    cmb_trigger->setSelectedIndex(type);
}

void TriggerSet::setValue(double val)
{
    char text[16];
    sprintf(text, "%.2lf", val);
    value->setText( text );
}

void TriggerSet::setOperation(int op)
{
    cmb_operation->setSelectedIndex(op);
}

void TriggerSet::draw()
{
	cmb_trigger->setPosition(this->x, this->y);
	cmb_operation->setPosition(this->x, cmb_trigger->getY()+cmb_trigger->getHeight()+1);
	value->setPosition(this->x,cmb_operation->getY()+cmb_operation->getHeight()+1);

	value->draw();
	cmb_operation->draw();
	cmb_trigger->draw();
}

INPUT_EVENT TriggerSet::input(SDL_Event &event)
{
	INPUT_EVENT e = NO_EVENT;

    e = cmb_trigger->input(event);
	if (e != NO_EVENT)
        return e;

    e = cmb_operation->input(event);
	if (e != NO_EVENT)
        return e;

    e = value->input(event);
    if (e != NO_EVENT)
        return e;

	return NO_EVENT;
}

void TriggerSet::setActive(bool state)
{
    cmb_trigger->setActive(state);
    cmb_operation->setActive(state);
    value->setActive(state);
    active = state;
}
