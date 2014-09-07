#include <cassert>
#include <SoundManager.hpp>

#define PI 3.14159265
#define RELDIST(a, b) ( (a > b) ? (a - b) : (b - a) ) //Calcula a distancia em uma dimensão
#define DISTANCE(x1, y1, x2, y2) sqrt( (double)RELDIST(x1, x2) * RELDIST(x1, x2) + RELDIST(y1, y2) * RELDIST(y1, y2) ) //Calcula a distancia em 2 dimensões
#define degrees(a) ( (int)(a * 180.0/PI ) ) //Converte um ângulo em radianos para graus.
#define REMOVE //

#ifdef NDEBUG
#define REMOVE
#endif

SoundManager::SoundManager(int frequency, Uint16 format, int superchannels, int chunksize) { 
	if ( SoundManager::smg == NULL ) {
		init(frequency, format, superchannels, chunksize);
		SoundManager::smg = this;
	}
}
SoundManager::~SoundManager(){ terminate(); }

void SoundManager::init(int frequency, Uint16 format, int channels, int chunksize) {
    bool isOpen = Mix_OpenAudio( frequency, format, channels, chunksize ) != -1;
    channels = ( (channels < 2) ? 1:2 );
    assert( isOpen );
    this->currentFrequency = frequency;
    this->currentFormat = format;
}

void SoundManager::terminate() {
    int i,
        calls = Mix_QuerySpec( NULL, NULL, NULL );
    for ( i = calls; i > 0; i-- )  Mix_CloseAudio();
    SoundManager::smg = NULL;
}

int SoundManager::getFormat(int *calls, int *frequency, Uint16 *format, int *channels) {
    int nCalls = Mix_QuerySpec( frequency, format, channels );
    if ( calls != NULL )    *calls = nCalls;
    return nCalls;
}

//CANAS INICIO
void SoundManager::allocateChannels(int numchans) {    Mix_AllocateChannels(numchans);     }

int SoundManager::getChannelNum() { return Mix_AllocateChannels(-1); }

bool SoundManager::isPlaying(int channel) { return Mix_Playing(channel); }

int SoundManager::getPlayingCount() { return Mix_Playing(-1); }

int SoundManager::reserveChannels(int num) { return Mix_ReserveChannels(num); }

void SoundManager::play(Mix_Chunk *chunk, int channel, int nTimes) {
    if( Mix_PlayChannel(channel, chunk, nTimes) == -1)
        REMOVE std::cout << "Mixer: Error playing at channel " << channel << ".\n";
}

void SoundManager::loop(Mix_Chunk *chunk) {
    if( Mix_PlayChannel(-1, chunk, -1) == -1)
        REMOVE std::cout << "Mixer: Error playing chunk.\n";
}

void SoundManager::pause(int channel) { Mix_Pause(channel); }
void SoundManager::pauseAllChannel() { Mix_Pause(-1); }

void SoundManager::resume(int channel) { Mix_Resume(channel); }
void SoundManager::resumeAllChannel() { Mix_Resume(-1); }

void SoundManager::stop(int channel){ Mix_HaltChannel(channel); }
void SoundManager::stopAllChannel(){ Mix_HaltChannel(-1); }
void SoundManager::stopChannelAfter(int channel, int milisec){ Mix_ExpireChannel(channel, milisec); }

void SoundManager::fadeOut(int channel, int milisec){ Mix_FadeOutChannel(channel, milisec); }
void SoundManager::fadeOutAllChannel(int milisec){ Mix_FadeOutChannel(-1, milisec); }

int SoundManager::getVol(int channel){ return Mix_Volume(channel, -1); }
void SoundManager::setVol(int channel, int vol){ Mix_Volume(channel, vol); }
void SoundManager::setAllChannelVol(int vol){ Mix_Volume( -1, vol); }

void SoundManager::setPos(int channel, Sint16 angle, Uint8 distance){
    if ( Mix_SetPosition( channel, angle, distance ) == 0 )
        REMOVE std::cout << "Mixer: Error setting channel " << channel << " position.\n";
}

void SoundManager::setDefaultPos(int channel){ SoundManager::setPos(channel, 0, 0); }

void SoundManager::setPanning(int channel, Uint8 pos){
    if ( pos > 254 )   pos = 254;
    if ( Mix_SetPanning( channel, 254 - pos, pos ) == 0 )
        REMOVE std::cout << "Mixer: Error setting panning for channel " << channel << " .\n";
}

void SoundManager::setDefaultPanning(int channel){
    if ( Mix_SetPanning( channel, 255, 255 ) == 0 )
        REMOVE std::cout << "Mixer: Error setting panning for channel " << channel << " to default.\n";
}

void SoundManager::setReverseStereo(int channel, bool activated){
    if ( Mix_SetReverseStereo(channel, (int)activated) == 0 )
        REMOVE std::cout << "Mixer: Error adjusting reverse stereo on channel " << channel << " .\n";
}
//CANAIS FIM

//CHUNK INICIO
void SoundManager::load(Mix_Chunk *chunk, const char *filename){
    chunk = Mix_LoadWAV(filename);
    assert(chunk != NULL);
}

void SoundManager::free(Mix_Chunk *chunk){  Mix_FreeChunk(chunk); }

int SoundManager::getVol(Mix_Chunk *chunk){ return Mix_VolumeChunk(chunk, -1); }

void SoundManager::setVol(Mix_Chunk *chunk, int vol){ Mix_VolumeChunk(chunk, vol); }
//CHUNK FIM

//MÚSICAS INICIO
bool SoundManager::isPlaying(){
    if ( Mix_PlayingMusic() && !Mix_PausedMusic() ) return true;
    return false;
}

void SoundManager::load(Mix_Music *pointer, const char *filename){
    pointer = Mix_LoadMUS(filename);
    assert( pointer != NULL );
}

void SoundManager::free(Mix_Music *pointer){ Mix_FreeMusic(pointer); }

void SoundManager::play(Mix_Music *music, int nTimes){
    if ( Mix_PlayMusic(music, nTimes) == -1 )   REMOVE std::cout << "Mixer: Error playing music.\n" << nTimes << " times\n";
}

void SoundManager::loop(Mix_Music *music){ if ( Mix_PlayMusic(music, -1) == -1 )   REMOVE std::cout << "Mixer: Error looping music.\n"; }

void SoundManager::pause(){ Mix_PauseMusic(); }

void SoundManager::resume(){ Mix_ResumeMusic(); }

void SoundManager::stop(){ Mix_HaltMusic(); }

void SoundManager::fadeIn(Mix_Music *music, int nTimes, int milisec){
    if ( Mix_FadeInMusic(music, nTimes, milisec) == -1 )    REMOVE std::cout << "Mixer: Error fading in music.\n";
}

void SoundManager::fadeInLoop(Mix_Music *music, int milisec){ SoundManager::fadeIn(music, -1, milisec); }

void SoundManager::fadeOut(int milisec){
    if( Mix_FadeOutMusic(milisec) == 0 )    REMOVE std::cout << "Mixer: Error fading out music.\n";
}

void SoundManager::setVol(int vol){ Mix_VolumeMusic(vol); }

int SoundManager::getVol(){ return Mix_VolumeMusic(-1); }
//MÚSICAS FIM

int SoundManager::getAngle(int xC, int yC, int x2, int y2){
    double rad;
    int angle;
    if ( x2 > xC ){
        rad = atan( (double)RELDIST(y2, yC)/RELDIST(x2, xC) );
        if ( y2 > yC )  angle = 90 + degrees(rad);
        else    angle = 90 - degrees(rad);
    }
    else {
        if( y2 == yC )  rad = 0;
        else    rad = atan( (double)RELDIST(x2, xC)/RELDIST(y2, yC) );
        if ( y2 > yC )  angle = 180 + degrees(rad);
        else    angle = 360 - degrees(rad);
    }
    REMOVE std::cout << "Calculated angle: " << angle << " degrees.\n";
    return angle;
}

int SoundManager::getDistance(int xL, int yL, int x2, int y2){
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    double maxAudibleDist = DISTANCE(info->current_w/2, info->current_h/2, info->current_h, info->current_w);
    int distance = (int)(127.0 * DISTANCE(xL, yL, x2, y2)/maxAudibleDist);

    if ( distance < 1 )    distance = 1;

    REMOVE std::cout << "Calculated distance: " << distance << " distance.\n";

    return distance;
}

SoundManager SoundManager::smg = NULL;