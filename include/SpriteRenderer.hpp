#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H
#include <vector>
#include <algorithm>
#include "Sprite.hpp"
class SpriteRenderer {
    public:
    SpriteRenderer();
    ~SpriteRenderer();
    void addSprite(Sprite* sprite);
    void removeSprite(Sprite* sprite);
    void clear();
    void render(SDL_Renderer* renderer);
    private:
    void applyRemovals();
    std::vector<Sprite*> spriteList;
    std::vector<Sprite*> pendingRemovals;
};

#endif