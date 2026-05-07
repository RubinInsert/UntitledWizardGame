#include "engine/core/TimeManager.hpp"
#include <SDL3/SDL.h>
void TimeManager::update() {
    Uint64 currentTime = SDL_GetTicksNS();

    if (lastTime == 0) {
        deltaTime = 0.0;
        elapsedTime = 0.0;
    } else {
        deltaTime = static_cast<double>(currentTime - lastTime) / 1000000000.0; // Convert from NS to Seconds
        elapsedTime += deltaTime;
    }

    lastTime = currentTime;
}

double TimeManager::getDeltaTime() const {
    return deltaTime;
}
double TimeManager::getElapsedTime() const {
    return elapsedTime;
}