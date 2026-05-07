#include "engine/render/SpriteRenderer.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include <SDL3/SDL.h>
SpriteRenderer::SpriteRenderer() {}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::render(const entt::registry& registry, SDL_Renderer* renderer, const Camera& camera) {
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

        // Calculate the top left corner of the sprite in world space
        float worldX = transform.position.x - transform.size.x * 0.5f;
        float worldY = transform.position.y - transform.size.y * 0.5f;
        SDL_FRect destRect{ (worldX - camera.position.x) * camera.zoom,
                            (worldY - camera.position.y) * camera.zoom,
                            transform.size.x * camera.zoom,
                            transform.size.y * camera.zoom};
        SDL_RenderTexture(renderer, sprite.texture, &sprite.srcRect, &destRect);
    }
}