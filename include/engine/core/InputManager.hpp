#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <SDL3/SDL.h>
#include <unordered_set>
class InputManager {
public:
    InputManager();
    void update();

    // Keyboard Queries
    bool isKeyDown(SDL_Keycode key) const;
    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyReleased(SDL_Keycode key) const;

    // Mouse Queries
    bool isMouseButtonDown(Uint8 button) const;
    bool isMouseButtonPressed(Uint8 button) const;
    bool isMouseButtonReleased(Uint8 button) const;
    SDL_FPoint getMousePosition() const;
    SDL_FPoint getPreviousMousePosition() const;

    bool shouldQuit() const;
    void reset();
private:
    std::unordered_set<SDL_Keycode> currentKeys;
    std::unordered_set<SDL_Keycode> previousKeys;
    
    Uint32 currentMouseButtons;
    Uint32 previousMouseButtons;
    
    SDL_FPoint mousePosition;
    SDL_FPoint previousMousePosition;
    bool quitRequested;
};

#endif