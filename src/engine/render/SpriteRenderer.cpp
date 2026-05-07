#include "engine/render/SpriteRenderer.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include <SDL3/SDL.h>
SpriteRenderer::SpriteRenderer() {}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::render(const entt::registry& registry, SDL_Renderer* renderer, const Camera& camera, const WorldSettings& worldSettings) {
    auto view = registry.view<Transform, Sprite>();
    std::vector<std::pair<entt::entity, float>> sortedEntities;
    for (auto object : view) {
        auto& transform = view.get<Transform>(object);
        float depthKey = transform.position.y + transform.size.y * 0.5f; // Get bottom of sprite
        sortedEntities.emplace_back(object, depthKey);
    }
    // Sort by Y position (ascending: lower Y renders first, higher Y on top)
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // Render the sorted entities (so "closer" sprites are rendered above further away sprites)
    for (const auto& [entity, y] : sortedEntities) {
        auto& transform = view.get<Transform>(entity);
        auto& sprite = view.get<Sprite>(entity);

        // Isometric projection
        float isoX = (transform.position.x - transform.position.y) * (worldSettings.tileWidth * 0.5f);
        float isoY = (transform.position.x + transform.position.y) * (worldSettings.tileHeight * 0.5f);
        
        // Camera offset
        // Calculate the top left corner of the sprite in world space
        float screenX = (isoX - camera.position.x) * camera.zoom + camera.viewportWidth * 0.5f;
        float screenY = (isoY - camera.position.y) * camera.zoom + camera.viewportHeight * 0.5f;

        // Draw sprite centered horizontally, anchored to feet/bottom
        SDL_FRect destRect{
            screenX - transform.size.x * 0.5f * camera.zoom,
            screenY - transform.size.y * camera.zoom,
            transform.size.x * camera.zoom,
            transform.size.y * camera.zoom
        };
        SDL_RenderTexture(renderer, sprite.texture, &sprite.srcRect, &destRect);
    }
}