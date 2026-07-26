#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H
#include <SDL3/SDL.h>
#include <vector>
#include "engine/core/WorldSettings.hpp"
#include "engine/render/Coordinate.hpp"
class DebugDrawer {
public:
    // Called once at the start of frame (before game calculations)
    static void Clear() {
        renderQueue.clear();
    }

    // Add a box to the queue
    static void AddBox(float x, float y, float w, float h, SDL_Color color = {255, 0, 0, 255}) {
    renderQueue.push_back({ {x, y, w, h}, color });
    }

    static void Draw(SDL_Renderer* renderer, const Camera& camera, const WorldSettings& settings) {
    for (const auto& entry : renderQueue) {
        // Project into Isometric Space & Apply camera transformations
        SDL_FPoint screen = Coordinate::WorldToScreen(entry.rect.x, entry.rect.y, camera, settings);
        // Create the Rect (Matches SpriteRenderer's centering logic)
        float visualW = entry.rect.w * settings.tileWidth;
        float visualH = entry.rect.h * settings.tileHeight;

        SDL_FRect destRect = {
            screen.x - (visualW * 0.5f),
            screen.y - (visualH * 0.5f),
            visualW,
            visualH
        };

        SDL_SetRenderDrawColor(renderer, entry.color.r, entry.color.g, entry.color.b, entry.color.a);
        SDL_RenderRect(renderer, &destRect);
    }
}
static void DrawIsometric(SDL_Renderer* renderer, const Camera& camera, const WorldSettings& settings) {
    for (const auto& entry : renderQueue) {
        // Calculate the 4 corners of the isometric diamond
        // Top, Right, Bottom, Left
        SDL_FPoint points[5]; // 5 points to close the loop
        
        float centerX = entry.rect.x;
        float centerY = entry.rect.y;
        float halfW = entry.rect.w * 0.5f; 
        float halfH = entry.rect.h * 0.5f;
        
        points[0] = Coordinate::WorldToScreen(centerX - halfW, centerY + halfH, camera, settings); // Top-Left
        points[1] = Coordinate::WorldToScreen(centerX + halfW, centerY + halfH, camera, settings); // Top-Right
        points[2] = Coordinate::WorldToScreen(centerX + halfW, centerY - halfH, camera, settings); // Bottom-Right
        points[3] = Coordinate::WorldToScreen(centerX - halfW, centerY - halfH, camera, settings); // Bottom-Left
        points[4] = points[0]; // Close loop
        SDL_SetRenderDrawColor(renderer, entry.color.r, entry.color.g, entry.color.b, entry.color.a);
        SDL_RenderLines(renderer, points, 5);
    }
}

private:
    struct DebugRect {
        SDL_FRect rect;
        SDL_Color color;
    };
    static inline std::vector<DebugRect> renderQueue;

    
};

#endif