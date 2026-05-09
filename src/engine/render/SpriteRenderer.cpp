#include "engine/render/SpriteRenderer.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/render/Coordinate.hpp"
#include <SDL3/SDL.h>
SpriteRenderer::SpriteRenderer() {}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::render(const entt::registry& registry, SDL_Renderer* renderer, const Camera& camera, const WorldSettings& worldSettings) {
    auto view = registry.view<Transform, Sprite>();
    std::vector<std::pair<entt::entity, float>> sortedEntities;
    for (auto object : view) {
        auto& transform = view.get<Transform>(object);
        // For depth sorting we convert to grid Y (down-positive) used by iso math.
        SDL_FPoint iso = Coordinate::WorldToIso(transform.position.x, transform.position.y, worldSettings);
        float depthKey = iso.y; // Get bottom of sprite in Isometric space
        sortedEntities.emplace_back(object, depthKey);
    }
    // Sort by Y position (ascending: lower Y renders first, higher Y on top)
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // Render the sorted entities (so "closer" sprites are rendered above further away sprites)
    for (const auto& [entity, y] : sortedEntities) {
        auto& transform = view.get<Transform>(entity);
        auto& sprite = view.get<Sprite>(entity);
        
        // Project from World to Screen for renderer      
        SDL_FPoint screen = Coordinate::WorldToScreen(transform.position.x, transform.position.y, camera, worldSettings);
        // We get the width/height from the actual frame source in the spritesheet
        const SDL_FRect& frameRect = sprite.src->getFrame(sprite.frame);
        float visualWidth = frameRect.w;
        float visualHeight = frameRect.h;

        // 4. DESTINATION RECT
        // screenX/screenY is the "anchor point" (the feet).
        // We center the sprite horizontally and sit it ON TOP of screenY.
        SDL_FRect destRect{
            screen.x - (visualWidth * 0.5f * camera.zoom), // Centered horizontally
            screen.y - (visualHeight * camera.zoom),        // Sits on the ground
            visualWidth * camera.zoom,
            visualHeight * camera.zoom
        };

        SDL_RenderTexture(renderer, sprite.src->getTexture(), &frameRect, &destRect);
    }
}