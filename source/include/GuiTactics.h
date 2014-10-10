#ifndef _GUI_TACTICS_H_
#define _GUI_TACTICS_H_

#include "GuiElements.h"
#include "Army.h"

// Editor de Trigger
class TriggerSet: public GuiElement
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
	void draw();

	void setType(int type);
	void setValue(double val);
	void setOperation(int op);

	INPUT_EVENT input(SDL_Event &event) override;
	void setActive(bool state) override;
};


class GuiTactic : public GuiElement
{
public:
	GuiTactic(int x, int y, std::string GID);
	~GuiTactic();

	virtual void update(){}
	virtual void update(int x, int y, DictKey *key){}
	virtual void draw(){}
	virtual INPUT_EVENT input(SDL_Event &event);

	virtual void setPartner( int id ){};
	virtual int getPartner(){ return 0; }
	virtual int getType() = 0;
};


class TacticSet : public GuiElement
{
    public:
        TacticSet(int x, int y, std::string GID);
        virtual ~TacticSet();

        void update();
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);
        Tactic* getTactic();
        int getType();
        void setTactic(int t, int id);
        void setTactic(Tactic *tactic, int id);


    protected:
        Box *box_container;
        ComboBox *cmb_choiser;
        TriggerSet *tg1, *tg2;
        ComboBox *cmb_logic;
        std::vector<std::string> vetor;
        GuiTactic *guiTactic;
        int unit_id;

    private:
        void setUItriggers(const TacticTrigger& trigger);

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
	TacticAN(int x, int y, std::string GID);
	virtual ~TacticAN();

	int getType() override {return TACTIC_ATTACK_NEAREST_ENEMY;}
};

/*Attack Weakest*/
class TacticAW : public GuiTactic
{
private:
public:
	TacticAW(int x, int y, std::string GID);
	virtual ~TacticAW();

	int getType() override {return TACTIC_ATTACK_WEAKEST_ENEMY;}
};

/*Collaborative Attack*/
class TacticCA : public GuiTactic
{
private:
    ComboBox *cmb_partner;
    int selfUnitID;

public:
	TacticCA(int x, int y, int id, int ally, std::string GID);
	virtual ~TacticCA();

	void update(){}
	void draw();
	INPUT_EVENT input(SDL_Event &event);

	int getPartner();
	void setPartner( int id );

	int getType() override {return TACTIC_ATTACK_COLLAB;}
};

/*Collaborative Defense*/
class TacticCD : public GuiTactic
{
private:
	ComboBox *cmb_partner;
    int selfUnitID;

public:
	TacticCD(int x, int y, int id, int ally, std::string GID);
	virtual ~TacticCD();

	void update(){}
	void draw();
	INPUT_EVENT input(SDL_Event &event);
	int getPartner();
	void setPartner( int id );

	int getType() override {return TACTIC_DEFENSE_COLLAB;}
};

/*Kamikase*/
class TacticKM : public GuiTactic
{
private:
public:
	TacticKM(int x, int y, std::string GID);
	virtual ~TacticKM();

	int getType() override {return TACTIC_KAMIKASE;}

};
/*Retreat*/
class TacticRT : public GuiTactic
{
private:
    ComboBox *cmb_partner;
    int selfUnitID;

public:
	TacticRT(int x, int y, int id, int ally, std::string GID);
	virtual ~TacticRT();

	void update(){}
	void draw();
	INPUT_EVENT input(SDL_Event &event);
	int getPartner();
	void setPartner( int id );

	int getType() override {return TACTIC_RETREAT;}
};

/*Random Movement*/
class TacticRM : public GuiTactic
{
private:
public:
	TacticRM(int x, int y, std::string GID);
	virtual ~TacticRM();

	int getType() override {return TACTIC_MOVE_RANDOM;}
};

//itemlist
class ItemList : public Label
{
private:
	int type;
	void initialize();
public:

	ItemList(std::string text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID);
	ItemList(int tp, std::string text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID);
	ItemList(int tp);
	ItemList();
	void setType(int i);
	~ItemList();
	void setSelected(bool selected);
	void draw();
	INPUT_EVENT input(SDL_Event &event);
};

//tacticlist
class TacticList: public GuiElement
{
private:
	Button *btn_up, *btn_down, *btn_plus, *btn_minus, *btn_show;
	std::vector<ItemList*> lista;
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
	void update();

	void draw();
	INPUT_EVENT input(SDL_Event &event);
};


#endif
