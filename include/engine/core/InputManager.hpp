#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <SDL3/SDL.h>
#include <unordered_set>
#include <functional>
#include "engine/core/IApplication.hpp"
class InputManager {
public:
    InputManager();
    /// @brief Updates the state of inputs
    /// @param eventCallback An optional void function callback that takes in an SDL_Event& to allow hooking onto any input events
    void update(IApplication* app = nullptr);

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