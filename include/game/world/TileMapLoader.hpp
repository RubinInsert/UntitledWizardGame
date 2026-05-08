#ifndef TILEMAPLOADER_H
#define TILEMAPLOADER_H
#include <string>
#include "game/world/TileMap.hpp"
struct LoadedTileSet {
    int columns;
    int rows;
    int margin;
    int spacing;
};
struct LoadedTileLayer {
    std::string name;
    int width;
    int height;
    std::vector<int> tileIds;
    std::string tilesetImagePath;
    int tilesetFirstGid;
    LoadedTileSet tileSet;
};

struct LoadedMap {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    std::vector<LoadedTileLayer> layers;
};



class TileMapLoader {
public:
    static LoadedMap loadTileMap(const std::string& mapFilePath);
};

#endif