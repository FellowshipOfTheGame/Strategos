
#include "Image.h"

#include "Global.h"
#include "Game.h"

bool compareColors(const SDL_Color color1, const SDL_Color color2)
{
    if (color1.r != color2.r) return false;
    if (color1.g != color2.g) return false;
    if (color1.b != color2.b) return false;

    //if (color1.unused != color2.unused) return false;
    return true;
}

///Usando o resource manager
Image::Image(SDL_Texture *image, int frames, SDL_Rect *rect, int cX, int cY)
    : centerX(cX), centerY(cY)
{
    //totalImages++; printf("Images: %i/%i\n", deletedImages, totalImages);

    if (rect != nullptr){
        cut = rect;
    }else{
        cut = new SDL_Rect[1];
        cut->x = cut->y = 0;
        SDL_QueryTexture(image, nullptr, nullptr, &(cut->w), &(cut->h));
//        cut->w = image->w;
//        cut->h = image->h;
    }

    nframes = frames;
    frameWidth = cut[0].w;
    frameHeight= cut[0].h;

	picture = image;
}

Image::~Image()
{
	delete [] cut;
	SDL_DestroyTexture(picture);
}

SDL_Texture* Image::getSDLTexture() const
{
    return picture;
}

void Image::DrawImage(SDL_Renderer *destination, SDL_Texture *source, SDL_Rect *clip, int x, int y)
{
	SDL_Rect offset={(Sint16)x, (Sint16)y};

	/*Draw on destination surface on position delimited by 'offset', the source surface clipped by the 'clip rect'*/
//	SDL_BlitSurface(source, clip, destination, &offset);
	SDL_RenderCopy(destination, source, clip, &offset);
}

void Image::DrawImage(SDL_Renderer *destination) const
{
	SDL_Rect offset={-centerX, -centerY, cut[0].w, cut[0].h};

	/*Draw on destination surface on position delimited by 'offset', the source surface clipped by the 'clip rect'*/
//	SDL_BlitSurface(picture, &cut[0], destination, &offset);
	SDL_RenderCopy(destination, picture, &cut[0], &offset);
}

void Image::DrawImage(SDL_Renderer *destination, int x, int y, int frame) const
{
    SDL_Rect offset;
	offset.x = x-centerX;
	offset.y = y-centerY;
	offset.w = cut[frame].w;
	offset.h = cut[frame].h;

//	SDL_BlitSurface(picture, &cut[frame], destination, &offset);
	SDL_RenderCopy(destination, picture, &cut[frame], &offset);
}

void Image::DrawImage(SDL_Renderer *destination, int x, int y, int frame, double rotation) const
{
	SDL_Rect offset;
	offset.x = x-centerX;
	offset.y = y-centerY;
	offset.w = cut[frame].w;
	offset.h = cut[frame].h;

	SDL_Point point;
	point.x = centerX;
	point.y = centerY;

//	SDL_BlitSurface(picture, &cut[0], destination, &offset);
	SDL_RenderCopyEx(destination, picture, &cut[frame], &offset, rotation, &point, SDL_FLIP_NONE);
}

int Image::getNumberFrames() const
{
    return nframes;
}

int Image::getFrameWidth() const
{
    return frameWidth;
}

int Image::getFrameHeight() const
{
    return frameHeight;
}

int Image::getCenterX() const
{
    return centerX;
}

int Image::getCenterY() const
{
    return centerY;
}

/** Um Clip e' utilizado para subdividir uma imagem em pedacos menores */
SDL_Rect *Image::createClip(int lines, int collumns, int clipWidth, int clipHeight, int margin, int padding)
{
	int ncuts = lines * collumns;
    int current = 0;
    int cutY = 0;

    SDL_Rect* cut = new SDL_Rect[ncuts];

    for(int i = 0; i < lines; i++)
    {
        cutY = i * (clipHeight + padding);

        if(i == 0)
            cutY += margin;

        for(int j = 0; j < collumns; j++, current++)
        {
            cut[current].w = clipWidth;
            cut[current].h = clipHeight;

            if(j == 0)
            {
                cut[current].x = 0 + margin;
            }
            else
            {
                cut[current].x = cut[current-1].x + cut[current-1].w + padding;
            }

			cut[current].y = cutY;

//			printf("Cut%d[%d, %d, %d, %d]\n", current, cut[current].x,
//                cut[current].y, cut[current].w, cut[current].h);
        }
    }

    return cut;
}

void Image::printCut(int frame)
{
    printf("PTR: %p -- ", this);
    printf("Cutting %d: [%d, %d, %d, %d]\n", frame, cut[frame].x, cut[frame].y,
                                    cut[frame].w, cut[frame].h);
}

Image* Image::cutFrame(int frame)
{
    if (frame > nframes){
        printf("Frame nao existe - %d. Limit: %d\n", frame, nframes);
        return nullptr;
    }

    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();

//    printCut(frame);

    // Criar surface
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, cut[0].w, cut[0].h);

    if(tex == nullptr)
    {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        return nullptr;
    }

    SDL_SetRenderTarget(Game::getGlobalGame()->getRenderer(), tex);
    SDL_RenderCopy(renderer, picture, &cut[frame], 0 );
    SDL_SetRenderTarget(Game::getGlobalGame()->getRenderer(), 0);

    // Desenhar na surface o frame correspondente
    SDL_Rect offset;
    offset.x = offset.y = 0;

    return new Image(tex, 1, nullptr, centerX, centerY);
}

Image** Image::generateRotatedImage(Image *source, float angle, int frames)
{
    Image **vec = new Image*[frames];
    double rot = angle/frames;
    double rotSum = 360;
    SDL_Point p;
    p.x = source->cut[0].w/2;
    p.y = source->cut[0].h/2;

    SDL_Renderer* renderer = Game::getGlobalGame()->getRenderer();
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    for (int i = 0; i < frames; ++i)
    {
        SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, source->cut[0].w, source->cut[0].h);
        SDL_SetRenderTarget(Game::getGlobalGame()->getRenderer(), tex);
        SDL_RenderCopyEx(renderer, source->picture, &source->cut[0], nullptr, rotSum, &p, flip );
        rotSum -= rot;

        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

        vec[i] = new Image( tex, 1, nullptr, source->centerX, source->centerY);
    }

    SDL_SetRenderTarget(renderer, 0);

    return vec;
}

