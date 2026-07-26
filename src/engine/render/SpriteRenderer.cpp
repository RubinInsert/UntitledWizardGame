#include "engine/render/SpriteRenderer.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/render/Coordinate.hpp"
#include <SDL3/SDL.h>
SpriteRenderer::SpriteRenderer() {}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::render(const entt::registry& registry, RenderSystem& renderSystem, const Camera& camera, const WorldSettings& worldSettings) {
    // TODO: Reimplement for 3D rendering pipeline
    // auto view = registry.view<Transform, Sprite>();
    // std::vector<std::pair<entt::entity, float>> sortedEntities;
    // for (auto object : view) {
    //     auto& transform = view.get<Transform>(object);
    //     auto& sprite = view.get<Sprite>(object);
    //     SDL_FPoint iso = Coordinate::WorldToIso(transform.position.x, transform.position.y, worldSettings);
    //     float depthKey = iso.y + sprite.depthOffset;
    //     sortedEntities.emplace_back(object, depthKey);
    // }
    // std::sort(sortedEntities.begin(), sortedEntities.end(),
    //     [](const auto& a, const auto& b) { return a.second < b.second; });

    // for (const auto& [entity, y] : sortedEntities) {
    //     auto& transform = view.get<Transform>(entity);
    //     auto& sprite = view.get<Sprite>(entity);
    //     SDL_FPoint screen = Coordinate::WorldToScreen(transform.position.x, transform.position.y, camera, worldSettings);
    //     const SDL_FRect& frameRect = sprite.src->getFrame(sprite.frame);
    //     float visualWidth = frameRect.w * transform.size.x;
    //     float visualHeight = frameRect.h * transform.size.y;
    //     SDL_FRect destRect{
    //         screen.x - (visualWidth * 0.5f),
    //         screen.y - (visualHeight),
    //         visualWidth,
    //         visualHeight
    //     };
    //     // 2D sprite draw call removed — adapting to 3D
    // }
}
