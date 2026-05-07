#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H
#include <vector>
#include <algorithm>
#include <entt/entt.hpp>
#include "engine/render/Camera.hpp"
#include "engine/core/WorldSettings.hpp"
class SpriteRenderer {
    public:
    SpriteRenderer();
    ~SpriteRenderer();
    void render(const entt::registry& registry, SDL_Renderer* renderer, const Camera& camera, const WorldSettings& worldSettings);
};

#endif