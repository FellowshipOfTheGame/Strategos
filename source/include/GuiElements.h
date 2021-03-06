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
	KEY_RELEASED_EVENT,
	MOUSE_OVER

} INPUT_EVENT;

class GuiElement
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
        GuiElement()
        {
            height = width = 0;
            x=y= 0;
            shown = enabled = active = 1;
        }

        virtual ~GuiElement(){}

        virtual void update(){}
        virtual void draw(){}

        virtual bool hover(int mouseX, int mouseY)
        {
            return (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height);
        }

        // TENTAR NAO UTILIZAR MAIS ESTE MODO!
        virtual bool hover()
        {
            int mouseX, mouseY;

            SDL_GetMouseState(&mouseX, &mouseY);

            return (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height);
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


class Button : public GuiElement
{
    private:
        Image *stringImg;
        int curFrame;
        int nFrames;
        const char *text;
        SDL_Color forecolor;
        SDL_Color backcolor;

        const Image *back;
        Font *font;

    public:
        Button(int x, int y, int width, int height, const Image *back);
        Button(int x, int y, const Image *back);
        virtual ~Button();

        virtual void update();
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);

        void setText(Font *font, const char *str, SDL_Color forecolor, SDL_Color backcolor);
};

class Box : public GuiElement
{
    protected:
        const Image *border;
        const Image *background;

    public:
        Box(int x, int y, int width, int height, const Image *background, const Image *border);
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
        ImageBox(int x, int y, int width, int height, const Image *background, int frame, const Image *border);
        virtual ~ImageBox();

        bool hover(int mouseX, int mouseY) override;
        bool hover() override;

        virtual void update();
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);
};

class TextField : public GuiElement
{
    private:
        std::string text;
        Image* imgText;
        unsigned int maxLength;
        bool caps;

        Font *font;
        const Image *border;
        SDL_Color background;

    public:
        TextField(int x, int y, int width, int height, const Image *border, SDL_Color background, std::string GID);
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

class ComboBox : public GuiElement
{
    private:
        std::vector<std::string> list;
        std::vector<Image*> listImg;
        int selected;
        bool drop, opened;

        const Image *border;
        Font *font;
        int cell_height;
        SDL_Color color;
        SDL_Color shadow;
        unsigned int lenght;

    public:
        ComboBox(int x, int y, const Image *border, SDL_Color fontColor, SDL_Color fontShadow);
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

class Label : public GuiElement
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
        void setColor(SDL_Color color);
        virtual INPUT_EVENT input(SDL_Event &event);

        void setText(std::string str);
};

class StatusBox : public GuiElement
{
    private:
        Box *bxBounds;
        const Image *background;

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
        StatusBox(int x, int y, const Image *imgBack, const Image *imgBorder, std::string GID);
        virtual ~StatusBox();

        virtual void update(){}
        void update(int x, int y, const DictKey *key);
        virtual void draw();
        virtual INPUT_EVENT input(SDL_Event &event);
};


#endif // _GUI_ELEMENTS_H_
