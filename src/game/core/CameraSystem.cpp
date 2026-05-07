#include "game/core/CameraSystem.hpp"
#include <cmath>
CameraSystem::CameraSystem() {}

void CameraSystem::update(Camera& camera, const InputManager& input) {
        SDL_FPoint mouse = input.getMousePosition();
        SDL_FPoint lastMouse = input.getPreviousMousePosition();
        SDL_FPoint delta{mouse.x - lastMouse.x, mouse.y - lastMouse.y};

        camera.position.x -= delta.x / camera.zoom;
        camera.position.y -= delta.y / camera.zoom;
}

void CameraSystem::followPlayer(Camera& camera, const Transform& playerTransform, const TimeManager& time) {
    float smoothing = 10.f * time.getDeltaTime();

    camera.position.x += (playerTransform.position.x - camera.position.x) * smoothing;
    camera.position.y += (playerTransform.position.y - camera.position.y) * smoothing;
}