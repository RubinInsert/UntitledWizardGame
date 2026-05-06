
#include "Sprite.hpp"
#include <SDL3/SDL.h>
Sprite::Sprite(std::string texturePath, AssetManager& assetManager, float x, float y)
{
    mTexture = assetManager.getTexture(texturePath);
    float textureWidth = 0.0f;
    float textureHeight = 0.0f;
    if(mTexture != nullptr) SDL_GetTextureSize(mTexture, &textureWidth, &textureHeight);
    srcRect = SDL_FRect{0.0f, 0.0f, textureWidth, textureHeight};
    destRect = SDL_FRect{x, y, textureWidth, textureHeight};
}
Sprite::~Sprite() {
    destroy();
}
void Sprite::destroy() {
    mTexture = nullptr;
}