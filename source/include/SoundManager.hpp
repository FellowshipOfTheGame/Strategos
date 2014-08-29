/**
*   O gerenciador de som é responsável por inicializar o sistema de som, alocar canais, obter o formato de som, alocar canais
* e aplicar efeitos sonoros, também calcula distância entre 2 pontos e o ângulo no sentido horário, partindo de "12 horas" para uso na função
* setChannelPostion.
*
*
*
*/

#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <cmath>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define DEFAULT_CHUNKSIZE 1024
#define DEFAULT_FADEIN_TIME 100
#define DEFAULT_FADEOUT_TIME 100

class SoundManager
{

    private:
        int currentFrequency; ///<Output frequency
        Uint16 currentFormat;  ///<Sound format

/* Formatos de som suportados
AUDIO_U8
    Unsigned 8-bit samples
AUDIO_S8
    Signed 8-bit samples
AUDIO_U16LSB
    Unsigned 16-bit samples, in little-endian byte order
AUDIO_S16LSB
    Signed 16-bit samples, in little-endian byte order
AUDIO_U16MSB
    Unsigned 16-bit samples, in big-endian byte order
AUDIO_S16MSB
    Signed 16-bit samples, in big-endian byte order
AUDIO_U16
    same as AUDIO_U16LSB (for backwards compatability probably)
AUDIO_S16
    same as AUDIO_S16LSB (for backwards compatability probably)
AUDIO_U16SYS
    Unsigned 16-bit samples, in system byte order
AUDIO_S16SYS (Padrão, equivale a MIX_DEFAULT_FORMAT)
    Signed 16-bit samples, in system byte order
*/

    public:
        SoundManager(int frequency = MIX_DEFAULT_FREQUENCY, Uint16 format = MIX_DEFAULT_FORMAT, int superchannels = MIX_DEFAULT_CHANNELS, int chunksize = DEFAULT_CHUNKSIZE);
        ~SoundManager();

        void init(int frequency, Uint16 format, int superchannels, int chunksize);
        ///<Initializes SDL_Mixer, "superchannels": 1 for mono and 2 for stereo, chunksize: size occupied by chunks loaded in the memory.
        void terminate(); ///<Terminates the SoundManager.

        int getFormat(int *calls, int *frequency, Uint16 *format, int *channels); ///<Fornece o formato de som atual do sistema.

        //CANAIS INICIO
        void allocateChannels(int numchans); ///<Allocate sound channels, in this case  they are not mono or stereo, they are the channels that play chunks.

        int getChannelNum(); ///<Returns the number of channels

        bool isPlaying(int channel);
        int getPlayingCount();

        int reserveChannels(int num); ///<Keeps num channels from doing anything without explicit orders (The AllChannel  functions won't work on them)

        void play(Mix_Chunk *chunk, int channel = -1, int nTimes = 0);

        void loop(Mix_Chunk *chunk);

        void pause(int channel); ///<Pauses the sound. Through resumeChannel() it continues to play.
        void pauseAllChannel();

        void resume(int channel);
        void resumeAllChannel();

        void stop(int channel); ///<Stops the sound and calls Mix_ChannelFinished if necessary (in this case resumeChannel() won't work ).
        void stopAllChannel();
        void stopChannelAfter(int channel, int milisec); ///<Stops channel after milisec.

        void fadeOut(int channel, int milisec); ///<Reduces channel volume to 0 during milisec.
        void fadeOutAllChannel(int milisec);

        int getVol(int channel);
        void setVol(int channel, int vol);
        void setAllChannelVol(int vol);

        void setPos(int channel, Sint16 angle, Uint8 distance); ///<Sets sound position relative to the listener.
        void setDefaultPos(int channel); ///<Deactivates position effect.

        void setPanning(int channel, Uint8 pos); ///<Sets channel panning, how intense sound will be on each "superchannel".
        void setDefaultPanning(int channel); ///<Deactivates panning effect.

        void setReverseStereo(int channel, bool activated); ///<Inverte os "supercanais" Esq <-> Dir ou  Dir<->Esq

        //CANAIS FIM

        //CHUNK INICIO
        void load(Mix_Chunk *chunk, const char *filename);
        void free(Mix_Chunk *chunk);

        int getVol(Mix_Chunk *chunk);
        void setVol(Mix_Chunk *chunk, int vol);
        //CHUNK FIM

        //MÚSICA INÍCIO
        bool isPlaying();

        void load(Mix_Music *pointer, const char *filename);
        void free(Mix_Music *pointer);

        void play(Mix_Music *music, int nTimes = 0);
        void loop(Mix_Music *music);

        void pause();

        void resume();

        void stop();

        void fadeIn(Mix_Music *music, int nTimes = 0, int milisec = DEFAULT_FADEIN_TIME); ///<Plays music while increasing it's volume from 0 to default, during milisec
        void fadeInLoop(Mix_Music *music, int milisec = DEFAULT_CHUNKSIZE);

        void fadeOut(int milisec = DEFAULT_FADEOUT_TIME);

        int getVol();
        void setVol(int vol);
        //MÚSICA FIM

        int getAngle(int x1, int y1, int x2, int y2); ///<Returns the angle where sound must be produced, from 12 hours, clockwise, it's close to polar coordinates.
        int getDistance(int xL, int yL, int x2, int y2); ///<Calculates the distance between the listener and the sound production, would be the radius of polar coordinates.

};

#endif
