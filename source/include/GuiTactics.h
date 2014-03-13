#ifndef _GUI_TACTICS_H_
#define _GUI_TACTICS_H_

#include "GuiElements.h"
#include "Army.h"
//trigger
class TriggerSet: public cGuiElement
{
private:
	ComboBox *cmb_trigger;
	TextField *value;
	ComboBox *cmb_operation;
	void initialize();
public:
	TriggerSet(int x, int y);
	~TriggerSet();
	Trigger* getTrigger();
	int getValue();
	int getOperation();
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};

class GuiTactic : public cGuiElement
{
public:
	GuiTactic(int x, int y, string GID);
	~GuiTactic();

	virtual void update(){}
	void update(int x, int y, DictKey *key);
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
	void setDistance(int i);
	int getDistance();
	int getPartner(){ return 0; }
};

class TacticSet : public cGuiElement
{
protected:
	Box *box_container;
	ComboBox *cmb_choiser;
	TriggerSet *tg1, *tg2;
	ComboBox *cmb_logic;
	std::vector<std::string> vetor;
	GuiTactic *guiTactic;
	int unit_id;
	Tactic *tactic;

public:
	TacticSet(int x, int y, string GID);
	virtual ~TacticSet();

	void update(int x, int y, DictKey *key);
	void update();
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
	Tactic* getTactic();
	int getType();
	void setTactic(Tactic *t, int id);
	void convert_AN();
	void convert_AW();
	void convert_CA(int id);
	void convert_CD(int id);
	void convert_KM();
	void convert_RT(int id);
	void convert_RM();
};


/*Attack Nearest*/
class TacticAN : public GuiTactic
{
private:
public:
	TacticAN(int x, int y, string GID);
	virtual ~TacticAN();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};

/*Attack Weakest*/
class TacticAW : public GuiTactic
{
private:
public:
	TacticAW(int x, int y, string GID);
	virtual ~TacticAW();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};

/*Collaborative Attack*/
class TacticCA : public GuiTactic
{
private:
ComboBox *cmb_partner;
public:
	TacticCA(int x, int y, int id, string GID);
	virtual ~TacticCA();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);

	int getPartner();
};

/*Collaborative Defense*/
class TacticCD : public GuiTactic
{
private:
	ComboBox *cmb_partner;
public:
	TacticCD(int x, int y, int id, string GID);
	virtual ~TacticCD();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
	int getPartner();

};

/*Kamikase*/
class TacticKM : public GuiTactic
{
private:
public:
	TacticKM(int x, int y, string GID);
	virtual ~TacticKM();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);

};
/*Retreat*/
class TacticRT : public GuiTactic
{
private:
		ComboBox *cmb_partner;
public:
	TacticRT(int x, int y, int id,string GID);
	virtual ~TacticRT();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
	int getPartner();

};

/*Random Movement*/
class TacticRM : public GuiTactic
{
private:
public:
	TacticRM(int x, int y, string GID);
	virtual ~TacticRM();

	void update(){}
	void update(int x, int y, DictKey *key);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};

//itemlist
class ItemList : public Label
{
private:
	int type;
	void initialize();
public:

	ItemList(string text, Font *font, SDL_Color color, SDL_Color shadow, string GID);
	ItemList(int tp,string text, Font *font, SDL_Color color, SDL_Color shadow, string GID);
	ItemList(int tp);
	ItemList();
	void setType(int i);
	~ItemList();
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};
//tacticlist
class TacticList: public cGuiElement
{
private:
	Button *btn_up, *btn_down, *btn_plus, *btn_minus, *btn_show;
	vector<ItemList*> lista;
	TacticSet *tct_set;
	ItemList *selected;
	Unit *squad;
	unsigned int top;
	void initialize();
public:
	TacticList(int x, int y);
	TacticList(Unit *squad, int x, int y);
	~TacticList();
	bool hover();
	void setSquad(Unit *squad);

	void draw();
	INPUT_EVENT input(SDL_Event &event);
};


#endif
