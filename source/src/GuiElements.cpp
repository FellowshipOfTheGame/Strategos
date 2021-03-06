
#include <vector>
#include <cassert>

#include "Global.h"
#include "Game.h"
#include "GuiElements.h"

#include "SDL2_gfx/SDL2_gfxPrimitives.h"

/****************************** BUTTON ******************************/
//a principio ta pronto
Button::Button(int x, int y, int width, int height, const Image *back)
{
    assert(back != nullptr);

    stringImg = 0;
	this->font = nullptr;
	this->text = nullptr;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	this->curFrame = 0;

    this->back = back;
    this->nFrames = back->getNumberFrames();

	this->enabled = true;
	this->active = true;
	this->shown = true;
}

Button::Button(int x, int y, const Image *back)
{
    assert(back != nullptr);

    stringImg = 0;
	this->font = nullptr;
	this->text = nullptr;
	this->x = x;
	this->y = y;

	this->curFrame = 0;

    this->back = back;
	this->width = back->getFrameWidth();
	this->height = back->getFrameHeight();
    this->nFrames = back->getNumberFrames();

	this->enabled = true;
	this->active = true;
	this->shown = true;
}

Button::~Button()
{
	//Nothing to be freed
}

void Button::update()
{
	if (hover())
	{
		curFrame = IMG_BUTTON_MOUSE_OVER;
	}
}

/* Assumptions:
 *  - The middle is determined by one frame only
 *  - The other 2 halfs can be composed by any number of frames
 *  - Hopefully baseFrame will be an odd number
 *  - curFrame already holds the correct frame (normal, hover, pressed)
 */
void Button::draw()
{
    back->DrawImage(Game::getGlobalGame()->getRenderer(), x, y, curFrame);
	int lim = 0;
	int middle = 0; //back->getBaseFrames() / 2;
	int frameWidth = back->getFrameWidth();
	int frameHeight = back->getFrameHeight();
	int posX = x, posY = y;
	int baseFrames = 1; // back->getBaseFrames();
	int bkpCurFrame = curFrame;

	//Draw the first half of the button pattern
	lim = middle;
	while (lim--)
	{
		back->DrawImage(Game::getGlobalGame()->getRenderer(), posX, posY, curFrame++);
		posX += frameWidth;
	}

	//Replicate the middle until the desired width is achieved
	lim = (width - 2 * (middle * frameWidth)) / frameWidth;
	for (int i = 0; i < lim; i++)
    {
		back->DrawImage(Game::getGlobalGame()->getRenderer(), posX, posY, curFrame);
		posX += frameWidth;
	}

	//Draw the second half of the button pattern
	if (baseFrames != 1)
	{
		lim = middle + 1;
		curFrame++;

		while (lim--)
		{
			back->DrawImage(Game::getGlobalGame()->getRenderer(), posX, posY, curFrame++);
			posX += frameWidth;
		}
	}

	//Render the text on the ?Middle?
	if (font != nullptr)
	{
		posX = x + width / 2 - (int)(strlen(text) * font->getPtSize()) / 2;
		posY += frameHeight / 4;

        stringImg->DrawImage(Game::getGlobalGame()->getRenderer(), posX, posY );
//		font->DrawText(posX, posY, text, forecolor, backcolor, Game::getGlobalGame()->getRenderer());
	}

	curFrame = bkpCurFrame;
}

INPUT_EVENT Button::input(SDL_Event &event)
{
    if (hover(event.button.x, event.button.y))
    {
        curFrame = IMG_BUTTON_MOUSE_OVER;
        switch (event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                curFrame = IMG_BUTTON_PRESSED;
                return MOUSE_PRESSED_EVENT;

            case SDL_MOUSEBUTTONUP:
                return MOUSE_RELEASED_EVENT;

            default:
                curFrame = IMG_BUTTON_NORMAL;
                break;
        }

        return MOUSE_OVER;
    }
    else
    {
        curFrame = IMG_BUTTON_NORMAL;
    }

	return NO_EVENT;
}

void Button::setText(Font *font, const char *str, SDL_Color forecolor, SDL_Color backcolor)
{
	this->font = font;
	this->text = str;

	SDL_Texture* t = font->renderText( Game::getGlobalGame()->getRenderer(), text, forecolor );
    if (t){
        stringImg = new Image(t, 1, nullptr, 0, 0);
    }

	this->forecolor = forecolor;
	this->backcolor = backcolor;
}

/****************************** BOX ******************************/

Box::Box(int x, int y, int width, int height, const Image *background, const Image *border)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->background = nullptr;
	this->border = nullptr;

	if (background != nullptr)
		this->background = background;
	else
	{
		printf("WARNING: Could not find background for box\n");
	}

	if (border != nullptr)
		this->border = border;
	else
	{
		printf("WARNING: Could not find border for box\n");
	}
	this->enabled = true;
	this->active = true;
	this->shown = true;
}

Box::~Box()
{
	//Nothing to be freed
}

void Box::update()
{
}

void Box::draw()
{
	SDL_Renderer *render = Game::getGlobalGame()->getRenderer();
	//int right = x + width;
	//int bottom = y + height;

	/*Desenha o fundo*/
	if (border != nullptr)
	{
		border->DrawImage(render, x, y);
	}
	if (background != nullptr)
	{
		background->DrawImage(render, x, y);
	}
}

INPUT_EVENT Box::input(SDL_Event &event)
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
/****************************** BOX ******************************/

ImageBox::ImageBox(int x, int y, int width, int height, const Image *background, int frame, const Image *border) :
		Box(x, y, width, height, background, border)
{
	this->frame = frame;
}
/****************************** BOX ******************************/
ImageBox::~ImageBox()
{
	//Nothing to be freed
}

void ImageBox::update()
{
}

void ImageBox::draw()
{
	SDL_Renderer *render = Game::getGlobalGame()->getRenderer();
	//int right = x + width;
	//int bottom = y + height;
	/*Desenha o fundo*/
	if (border){
		border->DrawImage(render, x, y);
	}

	if (background){
		background->DrawImage(render, x, y, frame);
	}
}

bool ImageBox::hover(int mouseX, int mouseY)
{
    if (background){
        const int xC = x-background->getCenterX();
        const int yC = y-background->getCenterY();
        return (mouseX > xC && mouseX < xC+width) && (mouseY > yC && mouseY < yC+height);
    }

    return (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height);
}

bool ImageBox::hover()
{
    int mouseX, mouseY;

    SDL_GetMouseState(&mouseX, &mouseY);

    if (background){
        const int xC = x-background->getCenterX();
        const int yC = y-background->getCenterY();
        return (mouseX > xC && mouseX < xC+width) && (mouseY > yC && mouseY < yC+height);
    }
    return (mouseX > x && mouseX < x+width) && (mouseY > y && mouseY < y+height);
}

INPUT_EVENT ImageBox::input(SDL_Event &event)
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

		return MOUSE_OVER;
	}
	return NO_EVENT;
}

/****************************** TEXT_FIELD ******************************/

TextField::TextField(int x, int y, int width, int height, const Image *border, SDL_Color background, std::string GID)
    : imgText(0)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	this->border = border;
	this->background = background;

	text = "";
	this->maxLength = 12;
	this->caps = false;
	this->active = false;
	this->shown = true;
	this->enabled = true;
	font = nullptr;
}

TextField::~TextField()
{

}

void TextField::draw()
{
	SDL_Renderer *renderer = Game::getGlobalGame()->getRenderer();
//	SDL_Rect back;
//
//	back.x = x;
//	back.y = y;
//	back.w = width;
//	back.h = height;

    rectangleRGBA( renderer, x, y, x+width, y+height, 0, 0, 255, 255 );
    //	SDL_FillRect(renderer, &back, SDL_MapRGB(scrSurface->format, 0, 0, 255));

    if (imgText)
        imgText->DrawImage(renderer, x+5, y);
}
void TextField::setText(std::string str)
{
	this->text = str;

    delete imgText;
	if (text.size() > 0)
        imgText = new Image(font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), this->background), 1, nullptr, 0, 0);
    else
        imgText = 0;

}
std::string TextField::getText()
{
	return text;
}

void TextField::update()
{

}

void TextField::setCaps(bool caps)
{
	this->caps = caps;
}

void TextField::enterLetter(char letter)
{
	if ((caps == true) && (letter >= 'a' && letter <= 'z'))
		letter -= ('a' - 'A');	//32 e' a diferenca do 'a' ao 'A' em ASCII

	/*Letras A..Z e a..z e Numeros 0..9*/
	if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z') || (letter >= '0' && letter <= '9') || (letter == '-'))
	{
		if (text.size() < maxLength)
			text.push_back(letter);
	}
	/*Backspace*/
	else if (letter == 8)
	{
		if (text.size() > 0)
			text.erase(--text.end());
	}

	delete imgText;
	if (text.size() > 0)
        imgText = new Image(font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), this->background), 1, nullptr, 0, 0);
    else
        imgText = 0;
}

INPUT_EVENT TextField::input(SDL_Event &event)
{
    if (!active && !hover()){
        return NO_EVENT;
    }

	switch (event.type)
	{
		case SDL_MOUSEBUTTONUP:
			if (enabled && hover())
			{
				this->active = true;
				return MOUSE_RELEASED_EVENT;
			}
			break;

		case SDL_KEYDOWN:
			if (active)
			{
				if (event.key.keysym.sym == SDLK_RSHIFT || event.key.keysym.sym == SDLK_LSHIFT)
					setCaps(true);

				enterLetter((char) event.key.keysym.sym);
				return KEY_PRESSED_EVENT;
			}
			break;

		case SDL_KEYUP:
			if (active)
			{
				if (event.key.keysym.sym == SDLK_CAPSLOCK)
					caps = !caps;
				return KEY_RELEASED_EVENT;
			}
			break;

		default:
			break;
	}

	return NO_EVENT;
}

void TextField::setFont(Font *font)
{
	this->font = font;

	delete imgText;
	if (text.size() > 0)
        imgText = new Image(font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), this->background), 1, nullptr, 0, 0);
    else
        imgText = 0;
}

/****************************** COMBO_BOX ******************************/

ComboBox::ComboBox(int x, int y, const Image *Border, SDL_Color fontColor, SDL_Color fontShadow)
{
	this->font = nullptr;
	this->drop = false;
	this->x = x;
	this->y = y;
	this->width = 100;
	this->height = 20;

	if (Border)
        this->cell_height = Border->getFrameHeight();

	this->selected = 0;

	this->border = Border;
	this->color = fontColor;
	this->shadow = fontShadow;

	this->active = true;
	this->shown = true;
	this->enabled = true;
	this->opened = false;
	lenght = 0;
}

ComboBox::~ComboBox()
{
    while (list.size() > 0)
    {
        list.erase(list.begin());
        delete listImg[0];
        listImg.erase(listImg.begin());
    }
}

void ComboBox::setFont(Font *font)
{
	this->font = font;
	if (!border)
        this->cell_height = font->getPtSize()+2;
}

INPUT_EVENT ComboBox::input(SDL_Event &event)
{
	int px, py;
	int aux;
	aux = selected;
	if (hover())
	{
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				//curFrame = back->getBaseFrames()*IMG_BUTTON_PRESSED;
				return MOUSE_PRESSED_EVENT;
				break;

			case SDL_MOUSEBUTTONUP:
				if (opened)
				{
					SDL_GetMouseState(&px, &py);
					selected = (py - y) / cell_height;

					if (selected == 0)
					{
						selected = aux;
					}
					else
					{
						selected--;
					}

//					printf("selected %d %lu '%s'\n", selected, list.size(), list[selected].c_str());

					this->height = cell_height;
					opened = false;
				}
				else
				{
					opened = true;
					this->height = (cell_height) * (int)(list.size()+1);
				}
				//curFrame = back->getBaseFrames()*IMG_BUTTON_MOUSE_OVER;
				return MOUSE_RELEASED_EVENT;
				break;

			default:
				//curFrame = back->getBaseFrames()*IMG_BUTTON_NORMAL;
				break;
		}

		return MOUSE_OVER;
	}

	return NO_EVENT;
}

void ComboBox::draw()
{
    SDL_Renderer *renderer = Game::getGlobalGame()->getRenderer();

	unsigned int i, j;
	if (list.size() > 0)
	{
		if (hover() && opened)
		{
			this->height = ((cell_height) * (int)(list.size() + 1));

			// Renderizar fundo quando aberto
			if (border != nullptr){
                for (i = 0; i < list.size()+1; i++){
                    border->DrawImage(renderer, x, y + cell_height*i, 0);
                    for (j = 0; j < lenght+1; j++){
                        border->DrawImage(renderer, x + border->getFrameWidth()*(j+1), y +cell_height*i, 1);
                    }
                }
			}
            else{
//                SDL_Rect rect;
//                rect.x = x;
//                rect.y = y;
//                rect.w = lenght*font->getPtSize();
//                rect.h = cell_height * (int)(list.size()+1) + 1;
//                SDL_RenderFillRect(renderer, &rect);
            }

            // Renderizar textos
			for (i = 0; i < list.size(); i++){
				listImg[i]->DrawImage(renderer, x, y + ((cell_height) * (i+1)));
			}
		}
		else
		{
			this->height = cell_height;
			if (border != nullptr)
			{
				border->DrawImage(renderer, x, y, 0);
				for (i = 0; i < lenght; i++){
					border->DrawImage(renderer, x + (border->getFrameWidth()*(i+1)), y, 1);
				}
				// Seta
				border->DrawImage(renderer, x + (border->getFrameWidth()*(i+1)), y, 2);
			}else{
//                SDL_Rect rect;
//                rect.x = x;
//                rect.y = y;
//                rect.w = lenght*font->getPtSize();
//                rect.h = cell_height;
//                SDL_RenderFillRect(renderer, &rect);
			}
		}

		if (selected >= 0)
            listImg[selected]->DrawImage(renderer, x, y);
	}
}

void ComboBox::update()
{
}

void ComboBox::addText(std::string str)
{
	if (str.size() > lenght)
		lenght = (int)str.size() + 1;
	this->list.push_back(str);

	if ( border )
        width = (lenght+2)*border->getFrameWidth();
	else
        width = (lenght+2)*font->getPtSize();

	Image *t = new Image(font->renderText(Game::getGlobalGame()->getRenderer(), str.c_str(), color), 1, nullptr, 0, 0);

	listImg.push_back(t);
}

std::string ComboBox::getText()
{
    if (selected < list.size())
        return this->list[this->selected];

    return std::string("---");
}

void ComboBox::removeText(int n)
{
    if (n < list.size())
    {
        list.erase(list.begin()+n);
        delete listImg[n];
        listImg.erase(listImg.begin()+n);
    }
}

int ComboBox::getSelectedIndex()
{
	return this->selected;
}

void ComboBox::setSelectedIndex(int i)
{
	this->selected = i;
}

void ComboBox::setSelected(std::string str)
{
	for (unsigned int i = 0; i < list.size() - 2; i++)
	{
		if (list[i] == str)
		{
			this->selected = i;
			break;
		}
	}
}

/****************************** LABEL ******************************/
//a principio ta pronto
Label::Label(std::string Text, Font *font, SDL_Color color, SDL_Color shadow, std::string GID)
    : text(Text)
{
	this->color = color;
	this->shadow = shadow;

	this->font = font;

	this->enabled = true;
	this->shown = true;
	this->active = true;

	this->imgText = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), color), 1, nullptr, 0, 0 );
	this->imgTextShadow = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), shadow), 1, nullptr, 0, 0 );
}

Label::~Label()
{
    delete imgText;
}
void Label::setColor(SDL_Color clor)
{
	delete this->imgText;
	delete this->imgTextShadow;
	this->color = clor;
	imgText = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), color), 1, nullptr, 0, 0 );
	this->imgTextShadow = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), shadow), 1, nullptr, 0, 0 );
}
void Label::draw()
{
    imgTextShadow->DrawImage(Game::getGlobalGame()->getRenderer(), x+1, y+1);
    imgText->DrawImage(Game::getGlobalGame()->getRenderer(), x, y);
}

INPUT_EVENT Label::input(SDL_Event &event)
{
	return NO_EVENT;
}

void Label::setText(std::string str)
{
	text.assign(str);

	delete this->imgText;
	delete this->imgTextShadow;

	this->imgText = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), color), 1, nullptr, 0, 0 );
	this->imgTextShadow = new Image( font->renderText(Game::getGlobalGame()->getRenderer(), text.c_str(), shadow), 1, nullptr, 0, 0 );

}

/****************************** STATUS_BOX ******************************/
//necessita que dictionary esteja funcionando
StatusBox::StatusBox(int x, int y, const Image *imgBack, const Image *imgBorder, std::string GID)
{
	width = 170;
	height = 150;
	background = imgBack;
	setPosition(x - 10, y - 10);
	//int right = left + height;
	//int bottom = top + width;

	bxBounds = new Box(getX(), getY(), width, height, imgBack, imgBorder);

	Font *font = Game::getGlobalGame()->getResourceMNGR()->GetFont("jostix-14");
	lbName = new Label("Name", font, ColorRGB8::Green, ColorRGB8::White, "LBL01" + GID);
	lbName->setPosition(getX() + 20, getY() + 70);

	lbHP = new Label("HP", font, ColorRGB8::Green, ColorRGB8::White, "LBL02" + GID);
	lbHP->setPosition(getX() + 20, getY() + 92);

	lbDamage = new Label("Damage", font, ColorRGB8::Green, ColorRGB8::White, "LBL03" + GID);
	lbDamage->setPosition(getX() + 20, getY() + 114);

	lbSpeed = new Label("Speed", font, ColorRGB8::Green, ColorRGB8::White, "LBL04" + GID);
	lbSpeed->setPosition(getX() + 20, getY() + 136);

	lbShield = new Label("Shield", font, ColorRGB8::Green, ColorRGB8::White, "LBL05" + GID);
	lbShield->setPosition(getX() + 20, getY() + 158);

	lbDodge = new Label("Dodge", font, ColorRGB8::Green, ColorRGB8::White, "LBL06" + GID);
	lbDodge->setPosition(getX() + 20, getY() + 180);

	lbRange = new Label("Range", font, ColorRGB8::Green, ColorRGB8::White, "LBL07" + GID);
	lbRange->setPosition(getX() + 20, getY() + 202);

	lbDescr = new Label("0", font, ColorRGB8::Green, ColorRGB8::White, "LBL08" + GID);

	active = false;
	shown = true;
	enabled = true;

	//this.GID = GID;
}

StatusBox::~StatusBox()
{
	delete bxBounds;
	delete lbName;
	delete lbHP;
	delete lbDamage;
	delete lbSpeed;
	delete lbShield;
	delete lbDodge;
	delete lbRange;
	delete lbDescr;
}

void StatusBox::draw()
{
	if (shown)
	{
		//bxBounds->draw();
		lbName->draw();
		lbHP->draw();
		lbDamage->draw();
		lbSpeed->draw();
		lbShield->draw();
		lbDodge->draw();
		lbRange->draw();

		/*Draws the description of each field*/
		/*for(unsigned int i = 0; i < 7; i++)
		 {

		 }*/
	}
}

void StatusBox::update(int x, int y, const DictKey *key)
{
	char aux[256];

	//printf ("nome %s\n",key->name);
	setPosition(x - 10, y - 10);
	sprintf(aux, "Name :%s", key->name);
	lbName->setText(aux);

	sprintf(aux, "HP: %0.1f", key->stats.maxHP);
	lbHP->setText(aux);

	sprintf(aux, "Damage: %0.1f", key->stats.damage);
	lbDamage->setText(aux);

	sprintf(aux, "Speed: %0.1f", key->stats.speed);
	lbSpeed->setText(aux);

	sprintf(aux, "Shield: %0.1f", key->stats.shield);
	lbShield->setText(aux);

	sprintf(aux, "Dodge: %0.1f", key->stats.dodge);
	lbDodge->setText(aux);

	sprintf(aux, "Range: %0.1f", key->stats.range);
	lbRange->setText(aux);

	lbName->setPosition(getX() + 20, getY() + 70);
	lbHP->setPosition(getX() + 20, getY() + 92);
	lbDamage->setPosition(getX() + 20, getY() + 114);
	lbSpeed->setPosition(getX() + 20, getY() + 136);
	lbShield->setPosition(getX() + 20, getY() + 158);
	lbDodge->setPosition(getX() + 20, getY() + 180);
	lbRange->setPosition(getX() + 20, getY() + 202);
}

INPUT_EVENT StatusBox::input(SDL_Event &event)
{

	return NO_EVENT;
}
