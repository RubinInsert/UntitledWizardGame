#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer() {}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::addSprite(Sprite* sprite) {
    if(!sprite) return;
    if(std::find(spriteList.begin(), spriteList.end(), sprite) == spriteList.end()) {
        spriteList.push_back(sprite);
    }
}
void SpriteRenderer::removeSprite(Sprite* sprite) {
    if (!sprite) return;
    if (std::find(pendingRemovals.begin(), pendingRemovals.end(), sprite) == pendingRemovals.end())
        pendingRemovals.push_back(sprite);
}
void SpriteRenderer::applyRemovals() { // Mitigates race conditions with render function by delaying removals until after render
    if (pendingRemovals.empty()) return;
    for (Sprite* p : pendingRemovals) {
        spriteList.erase(std::remove(spriteList.begin(), spriteList.end(), p), spriteList.end());
    }
    pendingRemovals.clear();
}
void SpriteRenderer::clear() {
    spriteList.clear();
    pendingRemovals.clear();
}
void SpriteRenderer::render(SDL_Renderer* renderer) {
    applyRemovals();
    for(Sprite* sprite : spriteList) {
        if(!sprite || !sprite->mTexture) continue;
        SDL_RenderTexture(renderer, sprite->mTexture, &sprite->srcRect, &sprite->destRect);
    }
}