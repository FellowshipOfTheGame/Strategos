
#ifndef _FONT_H_
#define _FONT_H_

//#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

class Font
{
    public:
        Font(TTF_Font *font_, int ptsize);
        ~Font();
        void CleanText();
        int getPtSize();
        void setShadow(bool t);

        SDL_Texture* renderText(SDL_Renderer* renderer, const char* text, SDL_Color textColor) const;

    private:
        bool useShadow;
        void Render(const char *text, SDL_Color textColor, SDL_Color shadowColor, SDL_Renderer* renderer);

    //    std::string myText;
        TTF_Font *font;
        const int ptsize;

        SDL_Color corText;
        SDL_Color corShadow;

        SDL_Surface *surfText;
        SDL_Surface *surfShadow;

        SDL_Texture *textureShadow;
        SDL_Texture *textureText;
};

#endif
