#include <vector>
#include <fstream>
#include <filesystem>
#include "game/world/Map.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/core/AssetManager.hpp"
#include "game/world/TileMap.hpp"
#include "game/world/MapLoader.hpp"
#include "engine/render/Coordinate.hpp"
#include "engine/render/DebugDrawer.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;
// Flags attached to the 32-bit global tile ID. Must be removed before checking tile type.
// Docs: https://doc.mapeditor.org/en/stable/reference/global-tile-ids/
const unsigned FLIPPED_HORIZONTALLY_FLAG  = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG    = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG    = 0x20000000;
const unsigned ROTATED_HEXAGONAL_120_FLAG = 0x10000000;
Map MapLoader::Load(std::string mapFilePath, AssetManager& assetManager, entt::registry& registry) {
    Map loadedMap;
    // Parse Map files
    fs::path mapPath = fs::path(mapFilePath); // Gets "assets/worldData/maps/map_file_name.json"
    fs::path mapDir = mapPath.parent_path(); // Gets "assets/worldData/maps/"

    // Try open file
    std::ifstream file(mapFilePath);
    if (!file.is_open()) {
        SDL_Log("Failed to open map file: %s", mapFilePath.c_str());
        return loadedMap;
    }

    // Parse Map File JSON
    json mapJson;
    try {
        file >> mapJson;
    } catch (const std::exception& e) {
        SDL_Log("JSON parse error: %s", e.what());
        return loadedMap;
    }

    // Parse map metadata
    loadedMap.width = mapJson["width"];
    loadedMap.height = mapJson["height"];
    loadedMap.tileWidth = mapJson["tilewidth"];
    loadedMap.tileHeight = mapJson["tileheight"];
    // Load TileSets
    std::unordered_map<int, Sprite> tileSets = LoadTileSets(mapJson, mapDir, assetManager);
    // Load TileMaps
    loadedMap.tileMap = LoadTileMap(mapJson);
    
    std::vector<entt::entity> loadedObjects = loadObjects(mapJson, tileSets, registry);

    loadedMap.tileMap.setGidAssetMap(std::move(tileSets));

    return loadedMap;
}

std::unordered_map<int, Sprite> MapLoader::LoadTileSets(json mapJSON, fs::path mapDir, AssetManager& assetManager) {
    std::unordered_map<int, Sprite> idToTileSprite;
    for (const auto& tilesetRef : mapJSON["tilesets"]) { // Loop through each tileset
        int firstGid = tilesetRef["firstgid"].get<int>();

        // Get the tilesets file path
        fs::path tilesetPath = mapDir / tilesetRef["source"].get<std::string>(); // "assets/worldData/maps/  ... /../tilesets/Tileset.json" == "assets/worldData/tilesets/Tileset.json"
        
        // read tileset file
        std::ifstream tilesetFile(tilesetPath);
        if(!tilesetFile.is_open()) {
            SDL_Log("Failed to open tileset: %s", tilesetPath.string().c_str());
            continue;
        }

        // Parse JSON from tileset file
        json tilesetJSON;
        try {
            tilesetFile >> tilesetJSON;
        } catch (const std::exception& e) {
            SDL_Log("Tileset parse error: %s", e.what());
            continue;
        }

        fs::path tilesetDir = tilesetPath.parent_path(); // Gets "assets/worldData/tilemaps/"
        int columns = tilesetJSON["columns"].get<int>();
        if (tilesetJSON.contains("image") && columns > 0) {
            // TYPE A: Standard Sheet (Your original code goes here safely)
            // Combines with tileset spritesheet file path
            fs::path imagePath = tilesetDir / tilesetJSON["image"].get<std::string>();
            int imageWidth = tilesetJSON["imagewidth"].get<int>();
            int imageHeight = tilesetJSON["imageheight"].get<int>();
            int tileCount = tilesetJSON["tilecount"].get<int>();
            int margin = tilesetJSON["margin"].get<int>();
            int spacing = tilesetJSON["spacing"].get<int>();
            int rows = tileCount / columns;
            int tileWidth = tilesetJSON["tilewidth"].get<int>();
            int tileHeight = tilesetJSON["tileheight"].get<int>();
            // Load sprite sheet for each given tileset
            SpriteSheet* spriteSheet = assetManager.getSpriteSheet(imagePath.string(), tileWidth, tileHeight, columns, rows, margin, margin, spacing, spacing);
            for(int x = 0; x < tileCount; x++) {
                int globalID = firstGid + x;
                Sprite tileSprite = Sprite{spriteSheet, x}; // Use the local ID to identify frame number
                idToTileSprite[globalID] = tileSprite; // Use global ID to identify tile -> sprite
                
            }
        } else if (tilesetJSON.contains("tiles")) {
            // TYPE B: Image Collection (For Objects)
            for (const auto& object : tilesetJSON["tiles"]) { // Loop through each "object" tile
                if (object.contains("image")) {
                    int localId = object["id"].get<int>();
                    fs::path individualPath = tilesetDir / object["image"].get<std::string>();

                    // Calculate the real global map ID: firstgid (17) + local index (0) = 17
                    int globalId = firstGid + localId;
                    int imageHeight = object["imageheight"];
                    int imageWidth = object["imagewidth"];
                    SpriteSheet* spriteSheet = assetManager.getSpriteSheet(individualPath.string(), imageWidth, imageHeight, 1, 1, 0, 0, 0, 0);
                    Sprite tileSprite = Sprite{spriteSheet, 0}; // In collection of images, there will only ever be one sprite per spritesheet so we hardcode 0.
                    // Store it directly in our flat asset lookup
                    idToTileSprite[globalId] = tileSprite;
                }
            }
        }
    }

    return idToTileSprite;
}

TileMap MapLoader::LoadTileMap(json mapJSON) {
    TileMap tileMap;
    int mapWidth = mapJSON["width"].get<int>();
    int mapHeight = mapJSON["height"].get<int>();
    tileMap.setWidth(mapWidth);
    tileMap.setHeight(mapHeight);
    for (const auto& layer : mapJSON["layers"]) {
        if(layer["type"].get<std::string>() != "tilelayer") continue; // Skip non-tile layers
        TileLayer tileLayer;
        tileLayer.name = layer["name"].get<std::string>();
        tileLayer.width = layer["width"].get<int>();
        tileLayer.height = layer["height"].get<int>();
        const auto& data = layer["data"];

        std::string usedImagePath;
        int usedFirstGid = 0;
        
        for(const auto& gidValue : data) {
            int rawGid = gidValue.get<int>();
            if (rawGid == 0) { // If empty tile
                tileLayer.tileData.push_back(-1);
                continue;
            }
            // Clear Tiled flags
            int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG | ROTATED_HEXAGONAL_120_FLAG);
            tileLayer.tileData.push_back(cleanGid); // Push Global tile ID
        }
        TileLayer* addedTileLayer = &tileMap.addLayer(std::move(tileLayer)); // Prevent deep copies by moving instead.
        if (layer.contains("class") && layer["class"].is_string()) {
            if (layer.contains("class") && layer["class"].is_string()) {
                if (layer["class"].get<std::string>() == "Collision") {
                    tileMap.setCollisionLayerIndex(static_cast<int>(tileMap.getLayers().size()) - 1);
                }
            }
        }
    }

    return tileMap;
}

std::vector<entt::entity> MapLoader::loadObjects(json mapJSON, const std::unordered_map<int, Sprite>& tileSets, entt::registry& registry) {
    std::vector<entt::entity> loadedObjects;
    if (!mapJSON.contains("layers")) return loadedObjects;
    for (const auto& layer : mapJSON["layers"]) {
        if (layer.value("type", "") != "objectgroup" || !layer.contains("objects")) continue;
        for (const auto& obj : layer["objects"]) {
            if (!obj.contains("gid")) continue; // We only care about visual objects
            int rawGid = obj["gid"].get<int>();
            int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | 
                                      FLIPPED_DIAGONALLY_FLAG | ROTATED_HEXAGONAL_120_FLAG);
            auto it = tileSets.find(cleanGid);
            if (it == tileSets.end()) continue; // Skip if asset wasn't found
            const Sprite& objectSprite = it->second;
            entt::entity loadedObject = registry.create();
            registry.emplace<Sprite>(loadedObject, objectSprite);
            // Extract positions from your JSON data
            float x = obj["x"].get<float>();
            float y = obj["y"].get<float>();
            float height = obj["height"].get<float>();
            float width = obj["width"].get<float>();
            SDL_FPoint objectPos = Coordinate::TiledIsoObjectToWorld(x, y, height);
            // Tiled anchors tile objects from the bottom-left, adjust Y to match top-left engines
            registry.emplace<Transform>(loadedObject, Transform{
                objectPos,
                SDL_FPoint{ 1, 1 }, // default to 1 x 1 size
                objectPos  // previousPosition (initially identical)
        });

            // Track the entity in your list
            loadedObjects.push_back(loadedObject);
        }
    }
    return loadedObjects;
}