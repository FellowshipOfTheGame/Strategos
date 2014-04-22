#include "global.h"
#include "Font.h"

/** Define uma fonte, isto e, seu typeface */
Font::Font(TTF_Font *font_, int ptsize_) :
		ptsize(ptsize_)
{
	//totalFonts++; printf("Fonts: %i/%i\n", deletedFonts, totalFonts);

	font = font_;

	corText = ColorRGB8::Red;
	corShadow = ColorRGB8::Black;
	surfText = nullptr;
	surfShadow = nullptr;
	useShadow = false;
}

Font::~Font()
{
	//deletedFonts++; printf("CLOSEFonts: %i/%i\n", deletedFonts, totalFonts);
	CleanText();
}

void Font::setShadow(bool t)
{
	useShadow = t;
}

void Font::Render(const char *text, SDL_Color textColor, SDL_Color shadowColor, SDL_Renderer* renderer)
{
	if (!text){
		return;
	}

	//Precisa refazer as texturas mesmo quando o texto nao mudar.
	//Pois qualquer outra imagem de fundo/frente pode estar mudando e o texto fica borrado

	/*Make sure the text has been cleaned before allocating on memory*/
	CleanText();

	if (font)
	{
		/*if (compareColors( corShadow, shadowColor ) == DIFERENTES)
		 {surfShadow = TTF_RenderUTF8_Blended(font, myText.c_str(), corShadow);}

		 if (compareColors( corText, textColor ) == DIFERENTES)
		 {surfText = TTF_RenderUTF8_Blended(font, myText.c_str(), corText);}*/

        surfShadow = TTF_RenderText_Blended(font, text, shadowColor);
        textureShadow = SDL_CreateTextureFromSurface(renderer, surfShadow);

        surfText = TTF_RenderText_Blended(font, text, textColor);
        textureText = SDL_CreateTextureFromSurface(renderer, surfText);
	}
}

SDL_Texture* Font::renderText(SDL_Renderer* renderer, const char* text, SDL_Color textColor) const
{
    SDL_Texture *texture = 0;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text, textColor);
    if (!surf)
        printf("Failded to create Text Surface: '%s'\n", text);
    else
        texture = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_FreeSurface(surf);

    return texture;
}

void Font::CleanText()
{
	if (surfText)
	{
		SDL_FreeSurface(surfText);
		SDL_DestroyTexture(textureText);
		surfText = nullptr;
		textureText = nullptr;
	}

	if (surfShadow)
	{
		SDL_FreeSurface(surfShadow);
		SDL_DestroyTexture(textureShadow);
		surfShadow = nullptr;
		textureShadow = nullptr;
	}
}

int Font::getPtSize()
{
	return ptsize;
}
