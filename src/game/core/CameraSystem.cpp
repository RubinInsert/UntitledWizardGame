#include "game/core/CameraSystem.hpp"
#include <cmath>
CameraSystem::CameraSystem() {}

void CameraSystem::update(Camera& camera, const InputManager& input) {
    if (input.isMouseButtonDown(SDL_BUTTON_LEFT)) {
        SDL_FPoint mouse = input.getMousePosition();
        SDL_FPoint lastMouse = input.getPreviousMousePosition();
        SDL_FPoint delta{mouse.x - lastMouse.x, mouse.y - lastMouse.y};

        camera.position.x -= delta.x / camera.zoom;
        camera.position.y -= delta.y / camera.zoom;
    } else {
        followPlayer(camera, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{5.f, 5.f}});
    }
}

void CameraSystem::followPlayer(Camera& camera, const Transform& playerTransform) {
    float smoothing = 0.01f;

    camera.position.x += (playerTransform.position.x - camera.position.x) * smoothing;
    camera.position.y += (playerTransform.position.y - camera.position.y) * smoothing;
}