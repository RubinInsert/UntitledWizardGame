#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H
#include <vector>
#include <algorithm>
#include <entt/entt.hpp>
#include "Sprite.hpp"
#include "Camera.hpp"
class SpriteRenderer {
    public:
    SpriteRenderer();
    ~SpriteRenderer();
    void render(const entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);
};

#endif