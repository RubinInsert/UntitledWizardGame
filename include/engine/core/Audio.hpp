#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>
struct Audio {
    Uint8* buffer;
    Uint32 length;
    SDL_AudioSpec spec;
};

#endif 