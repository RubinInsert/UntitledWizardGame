#include "game/core/CameraSystem.hpp"
#include "engine/render/Coordinate.hpp"
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
    
    // Convert to isometric world space (Without camera transformations)
    SDL_FPoint iso = Coordinate::WorldToIso(playerTransform.position.x, playerTransform.position.y, worldSettings);
    
    camera.position.x += (iso.x - camera.position.x) * smoothing;
    camera.position.y += (iso.y - camera.position.y) * smoothing;
}