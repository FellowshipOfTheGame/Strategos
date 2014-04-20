#ifndef _GUI_ELEMENTS_H_
#define _GUI_ELEMENTS_H_

#include <string>
#include "Image.h"
#include "Font.h"
#include "Dictionary.h"

#define IMG_BUTTON_NORMAL       0
#define IMG_BUTTON_MOUSE_OVER   1
#define IMG_BUTTON_PRESSED      2

typedef enum {
	NO_EVENT,
	eventBUTTON_PRESS,
	MOUSE_PRESSED_EVENT,
	MOUSE_RELEASED_EVENT,
	KEY_PRESSED_EVENT,
	KEY_RELEASED_EVENT

} INPUT_EVENT;

class cGuiElement
{
protected:
	int x;
	int y;
	int width;
	int height;
	bool shown;
	bool enabled;
	bool active;
	std::string GID;     //GID = Global Identifier ... ID que identifica univocamente cada componente grafico instanciado

public:
	cGuiElement()
    {
        height = width = 0;
        x=y= 0;
		shown = enabled = active = 1;
    }

    virtual ~cGuiElement(){}

	virtual void update(){}
	virtual void draw(){}

	virtual bool hover(int mouseX, int mouseY)
	{
		if( (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height) )
		{
			return true;
		}

		return false;
	}

    // TENTAR NAO UTILIZAR MAIS ESTE MODO!
    virtual bool hover()
	{
		int mouseX, mouseY;

		SDL_GetMouseState(&mouseX, &mouseY);

		if( (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height) )
		{
			return true;
		}

		return false;
	}

	//SETTERS / GETTERS
	virtual void setVisible(bool state) { shown = state; }
	virtual bool isVisible() { return shown; }
	virtual void setEnabled(bool state) { enabled = state; }
	virtual bool isEnabled() { return enabled; }
	virtual void setActive(bool state) { active = state; }
	virtual bool isActive() { return active; }
	virtual void setPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	virtual int getX() { return x; }
	virtual int getY() { return y; }
	virtual int getWidth() { return width; }
	virtual int getHeight() { return height; }

	//Manipula eventos necessarios
	virtual INPUT_EVENT input(SDL_Event &event){return NO_EVENT;}
};


class Button : public cGuiElement
{
    private:
        Image *stringImg;
        int curFrame;
        int nFrames;
        const char *text;
        SDL_Color forecolor;
        SDL_Color backcolor;

        Image *back;
        Font *font;

    public:
        Button(int x, int y, int width, int height, Image *back, std::string GID);
        virtual ~Button();

        virtual void update();
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);

        void setText(Font *font, const char *str, SDL_Color forecolor, SDL_Color backcolor);
};

class Box : public cGuiElement
{
protected:
	Image *border;
	Image *background;
public:
	Box(int x, int y, int width, int height, Image *background, Image *border, std::string GID);
	virtual ~Box();

	virtual void update();
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
};

class ImageBox : public Box
{
private:
	int frame;
public:
	ImageBox(int x, int y, int width, int height, Image *background, int frame, Image *border, std::string GID);
	virtual ~ImageBox();

	virtual void update();
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
};

class TextField : public cGuiElement
{
private:
	std::string text;
	Image* imgText;
	unsigned int maxLength;
	bool caps;

	Font *font;
	Image *border;
	SDL_Color background;

public:
	TextField(int x, int y, int width, int height, Image *border, SDL_Color background, std::string GID);
	virtual ~TextField();

	virtual void update();
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
	void setText(std::string str);
	std::string getText();
	void enterLetter(char letter);
	void setFont(Font *font);
	void setCaps(bool caps);
};

class ComboBox : public cGuiElement
{
private:
	std::vector<std::string> list;
	std::vector<Image*> listImg;
	int selected;
	bool drop, opened;

	Image *border;
	Font *font;
	int cell_height;
	SDL_Color color;
	SDL_Color shadow;
	unsigned int lenght;

public:
	ComboBox(int x, int y, int width, int height, Image *border, SDL_Color fontColor, SDL_Color fontShadow,std::string GID);
	//ComboBox(int x, int y, int width, int height, Image *border, string GID);
	virtual ~ComboBox();

	virtual void update();
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);

	void setFont(Font *font);
	void addText(std::string str);
	std::string getText();
	void removeText(int n);
	int getSelectedIndex();
	void setSelectedIndex(int i);
	void setSelected(std::string str);
};

class Label : public cGuiElement
{
    private:
        Font *font;
        std::string text;
        SDL_Color color;
        SDL_Color shadow;

        Image *imgText;
        Image *imgTextShadow;

    public:
        Label(std::string Text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID);
        virtual ~Label();

        //virtual void update();
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);

        void setText(std::string str);
};

class StatusBox : public cGuiElement
{
private:
	Box *bxBounds;
	Image *background;

	Label *lbName;
	Label *lbHP;
	Label *lbDamage;
	Label *lbSpeed;
	Label *lbShield;
	Label *lbDodge;
	Label *lbRange;
	Label *lbDescr;

	//DictKey *key;
	//vector<Label*> *lbDescr;
	std::string GID;

public:
	StatusBox(int x, int y, Image *imgBack, Image *imgBorder, std::string GID);
	virtual ~StatusBox();

	virtual void update(){}
	void update(int x, int y, const DictKey *key);
	virtual void draw();
	virtual INPUT_EVENT input(SDL_Event &event);
};


#endif // _GUI_ELEMENTS_H_
