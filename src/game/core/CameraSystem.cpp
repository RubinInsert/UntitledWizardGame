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

void CameraSystem::followPlayer(Camera& camera, const Transform& playerTransform, const TimeManager& time, const WorldSettings& worldSettings) {
    float smoothing = 10.f * time.getDeltaTime();
    
    // Isometric projection
    float targetX = (playerTransform.position.x - playerTransform.position.y) * (worldSettings.tileWidth * 0.5f);
    float targetY = (playerTransform.position.x + playerTransform.position.y) * (worldSettings.tileHeight * 0.5f);

    camera.position.x += (targetX - camera.position.x) * smoothing;
    camera.position.y += (targetY - camera.position.y) * smoothing;
}