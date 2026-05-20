#include "engine/render/SpriteSheet.hpp"
#include "engine/core/AssetManager.hpp"
SpriteSheet::SpriteSheet(AssetManager& assetManager, const std::string& assetPath, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY) {
    texture = assetManager.getTexture(assetPath);
    float texW = 0.0f, texH = 0.0f;
    SDL_GetTextureSize(texture, &texW, &texH);

    // If the image is smaller than our layout parameters, it's a fallback error asset
    if (texW < marginX + frameWidth) {
        // Map everything safely to the full bounds of the tiny error icon
            for (int row = 0; row < rows; ++row) {
                for (int col = 0; col < cols; ++col) {
                    frames.push_back(SDL_FRect{0.0f, 0.0f, texW, texH});
                }
            }
        
        return;
    }
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