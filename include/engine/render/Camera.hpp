#ifndef CAMERA_H
#define CAMERA_H
#include <SDL3/SDL.h>

struct Camera {
    SDL_FPoint position{0.f, 0.f};  // World position of camera
    float zoom{1.f};                 // 1.0 = 100%, 0.5 = 50% zoom out, 2.0 = 200% zoom in
    int viewportWidth{640};
    int viewportHeight{480};
};

#endif