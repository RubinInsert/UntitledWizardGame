#ifndef TILELAYER_H
#define TILELAYER_H
#include <vector>
#include <string>
struct TileLayer {
    /// @brief The raw Global IDs pulled from Tiled map JSON files.
    std::vector<int> tileData;

    std::string name; // Not necessarily useful as of right now.
    int width;
    int height;
    bool isVisible = true;
    float opacity = 1.0f;
};

#endif