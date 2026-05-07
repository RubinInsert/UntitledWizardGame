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

    float halfViewportX = camera.viewportWidth / (2.0f * camera.zoom);
    float halfViewportY = camera.viewportHeight / (2.0f * camera.zoom);

    float targetX = playerTransform.position.x - halfViewportX;
    float targetY = playerTransform.position.y - halfViewportY;

    camera.position.x += (targetX - camera.position.x) * smoothing;
    camera.position.y += (targetY - camera.position.y) * smoothing;
}