#include "engine/render/SpriteSheet.hpp"
#include "engine/core/AssetManager.hpp"
SpriteSheet::SpriteSheet(AssetManager& assetManager, const std::string& assetPath, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY) {
    texture = assetManager.getTexture(assetPath);
    frames.reserve(cols * rows);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            SDL_FRect frame{
                marginX + col * (frameWidth + spacingX),
                marginY + row * (frameHeight + spacingY),
                frameWidth,
                frameHeight
            };
            frames.push_back(frame);
        }
    }
}
SDL_Texture* SpriteSheet::getTexture() const {
    return texture;
}
const SDL_FRect& SpriteSheet::getFrame(int index) const {
    return frames[index];
}