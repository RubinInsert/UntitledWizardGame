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
        sortedEntities.emplace_back(object, transform.position.y);
    }
    // Sort by Y position (ascending: lower Y renders first, higher Y on top)
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // Render the sorted entities (so "closer" sprites are rendered above further away sprites)
    for (const auto& [entity, y] : sortedEntities) {
        auto& transform = view.get<Transform>(entity);
        auto& sprite = view.get<Sprite>(entity);
        SDL_FRect destRect{ transform.position.x - camera.position.x,
                            transform.position.y - camera.position.y,
                            transform.size.x * camera.zoom,
                            transform.size.y * camera.zoom};
        SDL_RenderTexture(renderer, sprite.texture, &sprite.srcRect, &destRect);
    }
}