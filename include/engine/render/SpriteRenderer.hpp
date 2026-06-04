#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H
#include <vector>
#include <algorithm>
#include <entt/entt.hpp>
#include "engine/render/Camera.hpp"
#include "engine/core/WorldSettings.hpp"
#include "engine/render/RenderSystem.hpp"
class SpriteRenderer {
    public:
    SpriteRenderer();
    ~SpriteRenderer();
    void render(const entt::registry& registry, RenderSystem& renderer, const Camera& camera, const WorldSettings& worldSettings);
};

#endif