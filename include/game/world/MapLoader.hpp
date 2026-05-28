#ifndef MAPLOADER_H
#define MAPLOADER_H
#include "game/world/Map.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <unordered_map>
#include <entt/entt.hpp>
using json = nlohmann::json;
namespace fs = std::filesystem;
class MapLoader {
    public:
    static Map Load(std::string mapFilePath, AssetManager& assetManager, entt::registry& registry);
    static std::unordered_map<int, Sprite> LoadTileSets(json mapJSON, fs::path mapDir, AssetManager& assetManager);
    static TileMap LoadTileMap(json mapJSON);
    static std::vector<entt::entity> loadObjects(json mapJSON, const std::unordered_map<int, Sprite>& tileSets, entt::registry& registry);
};

#endif