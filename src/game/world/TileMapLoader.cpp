#include "game/world/TileMapLoader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <SDL3/SDL.h>
using json = nlohmann::json;
namespace fs = std::filesystem;
// Flags attached to the 32-bit global tile ID. Must be removed before checking tile type.
// Docs: https://doc.mapeditor.org/en/stable/reference/global-tile-ids/
const unsigned FLIPPED_HORIZONTALLY_FLAG  = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG    = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG    = 0x20000000;
const unsigned ROTATED_HEXAGONAL_120_FLAG = 0x10000000;
LoadedMap TileMapLoader::loadTileMap(const std::string& mapFilePath) {
    LoadedMap result;
    
    fs::path mapPath = fs::path(mapFilePath); // Gets "assets/worldData/maps/map_file_name.json"
    fs::path mapDir = mapPath.parent_path(); // Gets "assets/worldData/maps/"

    // Try open file
    std::ifstream file(mapFilePath);
    if (!file.is_open()) {
        SDL_Log("Failed to open map file: %s", mapFilePath.c_str());
        return result;
    }

    // Parse Map File JSON
    json mapJson;
    try {
        file >> mapJson;
    } catch (const std::exception& e) {
        SDL_Log("JSON parse error: %s", e.what());
        return result;
    }

    // Parse map metadata
    result.width = mapJson["width"];
    result.height = mapJson["height"];
    result.tileWidth = mapJson["tilewidth"];
    result.tileHeight = mapJson["tileheight"];

    // Build tileset lookup: firstGid -> (imagePath, tilesetInfo)
    std::map<int, std::pair<std::string, LoadedTileSet>> tilesets;
    for (const auto& tilesetRef : mapJson["tilesets"]) {
        int firstGid = tilesetRef["firstgid"].get<int>();
        // concatnates "assets/worldData/maps/" with something like "../tilesets/tileset_example.json"
        // = "assets/worldData/tilesets/tileset_example.json"
        fs::path tilesetPath = mapDir / tilesetRef["source"].get<std::string>();

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
        // Combines with tileset spritesheet file path
        fs::path imagePath = tilesetDir / tilesetJSON["image"].get<std::string>();
        int columns = tilesetJSON["columns"].get<int>();
        int rows = tilesetJSON["tilecount"].get<int>() / columns;
        int margin = tilesetJSON["margin"].get<int>();
        int spacing = tilesetJSON["spacing"].get<int>();
        LoadedTileSet loadedTileSetInfo{columns, rows, margin, spacing};
        tilesets[firstGid] = { imagePath.lexically_normal().string(), loadedTileSetInfo };
    }
    for (const auto& layer : mapJson["layers"]) {
        if(layer["type"] != "tilelayer") {
            continue; // Support non-tile layers later
        }
        LoadedTileLayer tileLayer;
        tileLayer.name = layer["name"].get<std::string>();
        tileLayer.width = layer["width"].get<int>();
        tileLayer.height = layer["height"].get<int>();
        const auto& data = layer["data"];

        std::string usedImagePath;
        int usedFirstGid = 0;
        
        for(const auto& gidValue : data) {
            int rawGid = gidValue.get<int>();
            if(rawGid == 0) {
                continue; // Empty tile
            }
            // Clear the Tiled flags from the GID
            int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG | ROTATED_HEXAGONAL_120_FLAG);
            for(auto it = tilesets.rbegin(); it != tilesets.rend(); ++it) { // Reverse traversal (starting with largest "firstgid" tile set)
                if(it->first <= cleanGid) { // is the current tile's ID bigger than the tileset's first tileID (i.e. does the tile belong to this tileset?)
                    // Found the tile set that owns this tile
                    usedFirstGid = it->first;
                    usedImagePath = it->second.first;
                    tileLayer.tileSet = it->second.second;
                    break;
                }
            }
            break;
        }

        tileLayer.tilesetImagePath = usedImagePath;
        tileLayer.tilesetFirstGid = usedFirstGid;
        for (const auto& gidValue : data) { // Loop through tiles
            int rawGid = gidValue.get<int>();

            if (rawGid == 0) { // If empty tile
                tileLayer.tileIds.push_back(-1);
                continue;
            }
            // Clear Tiled flags
            int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG | ROTATED_HEXAGONAL_120_FLAG);
            tileLayer.tileIds.push_back(cleanGid - usedFirstGid); // Push Local tileset ID
        }

    result.layers.push_back(std::move(tileLayer));
    if(layer.contains("class") && layer["class"] == "Collision") {
        result.collisionLayerIndex = result.layers.size() - 1;
    }
    }
    return result;
}
