#ifndef MAPLOADER_H
#define MAPLOADER_H
#include "game/world/Map.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <unordered_map>
using json = nlohmann::json;
namespace fs = std::filesystem;
class MapLoader {
    public:
    static Map Load(std::string mapFilePath, AssetManager& assetManager);
    static std::unordered_map<int, Sprite> LoadTileSets(json mapJSON, fs::path mapDir, AssetManager& assetManager);
    static TileMap LoadTileMap(json mapJSON);
};

#endif