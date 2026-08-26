#include "engine/core/InputManager.hpp"
#include <SDL3/SDL.h>
InputManager::InputManager()
    : currentMouseButtons(0), previousMouseButtons(0), 
      mousePosition({0.f, 0.f}), previousMousePosition({0.f, 0.f}), quitRequested(false) {}

void InputManager::update(IApplication* app) {
            // Previous state of keys to determine if key is just pressed or is down.
            previousKeys = currentKeys;
            previousMouseButtons = currentMouseButtons;
            previousMousePosition = mousePosition;
            // The event data
            SDL_Event e;
            SDL_zero(e);
            while (SDL_PollEvent(&e) == true) {
              // Relay raw events directly to the application
              if (app) app->OnEvent(e);
                // If event is quit type
                switch(e.type) {
                    case SDL_EVENT_QUIT:
                        quitRequested = true;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        currentKeys.insert(e.key.key);
                        break;
                    case SDL_EVENT_KEY_UP:
                        currentKeys.erase(e.key.key);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        //SDL_MouseButtonFlags mouseButtonsDown = SDL_GetMouseState(&mousePosition.x, &mousePosition.y)
                        mousePosition.x = e.motion.x;
                        mousePosition.y = e.motion.y;
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        currentMouseButtons |= SDL_BUTTON_MASK(e.button.button);
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        currentMouseButtons &= ~SDL_BUTTON_MASK(e.button.button);
                        break;
                    default:
                        break;
                }
            }
      }
      bool InputManager::isKeyDown(SDL_Keycode key) const {
        return currentKeys.find(key) != currentKeys.end();
      }
      bool InputManager::isKeyPressed(SDL_Keycode key) const {
        bool isCurrentlyDown = currentKeys.find(key) != currentKeys.end();
        bool wasDown = previousKeys.find(key) != previousKeys.end();
        return isCurrentlyDown && !wasDown;
      }
      bool InputManager::isKeyReleased(SDL_Keycode key) const {
        bool isCurrentlyDown = currentKeys.find(key) != currentKeys.end();
        bool wasDown = previousKeys.find(key) != previousKeys.end();
        return !isCurrentlyDown && wasDown;
      }
      bool InputManager::isMouseButtonDown(Uint8 button) const {
        return (currentMouseButtons & SDL_BUTTON_MASK(button)) != 0;
      }
      bool InputManager::isMouseButtonPressed(Uint8 button) const {
        bool isCurrentlyDown = (currentMouseButtons & SDL_BUTTON_MASK(button)) != 0;
        bool wasDown = (previousMouseButtons & SDL_BUTTON_MASK(button)) != 0;
        return isCurrentlyDown && !wasDown;
      }
      bool InputManager::isMouseButtonReleased(Uint8 button) const {
        bool isCurrentlyDown = (currentMouseButtons & SDL_BUTTON_MASK(button)) != 0;
        bool wasDown = (previousMouseButtons & SDL_BUTTON_MASK(button)) != 0;
        return !isCurrentlyDown && wasDown;
      }
      SDL_FPoint InputManager::getMousePosition() const {
        return mousePosition;
      }
      SDL_FPoint InputManager::getPreviousMousePosition() const {
        return previousMousePosition;
      }
      bool InputManager::shouldQuit() const {
        return quitRequested;
      }
      void InputManager::reset() {
        currentKeys.clear();
        previousKeys.clear();
        currentMouseButtons = 0;
        previousMouseButtons = 0;
        mousePosition = {0.f, 0.f};
        previousMousePosition = {0.f, 0.f};
        quitRequested = false;
      }
