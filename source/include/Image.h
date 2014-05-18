
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>

#include <SDL.h>
#include <SDL_image.h>

class Image
{
    public:
        /**
        *   @param source Imagem inicial para gerar o vetor
        *   @param angle Angulo maximo - normalmente 360 graus
        *   @param frames Numero de frames para gerar, mais frames geram animacao mais suave
        *   @return Retorna um vetor contendo todos os frames da imagem source rotacionada
        */
        static Image** generateRotatedImage(Image *source, float angle, int frames);

        // Retorna um vetor de SDL_Rect
        static SDL_Rect *createClip(int lines, int collumns, int clipWidth, int clipHeight, int margin, int padding);

        // Desenha uma imagem
        static void DrawImage(SDL_Renderer *destination, SDL_Texture *source, SDL_Rect *clip, int x, int y);


        Image(SDL_Texture *image, int frames, SDL_Rect *rect, int cX, int cY);
        ~Image();
        void DrawImage(SDL_Renderer *destination) const;
        void DrawImage(SDL_Renderer *destination, int x, int y, int frame=0) const;
        void DrawImage(SDL_Renderer *destination, int x, int y, int frame, double rotation) const;

        int getNumberFrames() const;
        int getFrameWidth() const;
        int getFrameHeight() const;

        /**
         *
         */
        SDL_Texture* getSDLTexture();

        /**
        *   @param frame Numero do frame para ser cortado
        *   @return Retorna um novo Image que possui o frame cortado
        */
        Image* cutFrame(int frame);

        void printCut(int frame);

    private:
        SDL_Texture *picture;
        int frameWidth, frameHeight;
        int baseFrame;
        int nframes;
        int centerX, centerY;
        SDL_Rect *cut;

};

/* KURAMA: Apenas um Lembrete*/
/*SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
    if(!Surface || !Width || !Height)
        return 0;

    SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
                                             Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

    double    _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
    _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

    for(Sint32 y = 0; y < Surface->h; y++)
        for(Sint32 x = 0; x < Surface->w; x++)
            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
                for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
                    DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                              static_cast<Sint32>(_stretch_factor_y * y) + o_y, ReadPixel(Surface, x, y));

    return _ret;
}*/
/*****************************/

#endif
