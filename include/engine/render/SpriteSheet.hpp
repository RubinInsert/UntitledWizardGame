#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <vector>
#include <SDL3/SDL.h>
#include <string>
class AssetManager;
class SpriteSheet {
    // Extract grid of frames: frameWidth, frameHeight, cols, rows
    public:
    SpriteSheet(AssetManager& assetManager, const std::string& assetPath, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY);
    const SDL_FRect& getFrame(int index) const;
    SDL_Texture* getTexture() const;
    private:
    SDL_Texture* texture{nullptr};
    std::vector<SDL_FRect> frames;
};

#endif