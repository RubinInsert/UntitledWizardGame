#include "game/world/ObjectLoader.hpp"

using json = nlohmann::json;
// Flags attached to the 32-bit global tile ID. Must be removed before checking tile type.
// Docs: https://doc.mapeditor.org/en/stable/reference/global-tile-ids/
const unsigned FLIPPED_HORIZONTALLY_FLAG  = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG    = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG    = 0x20000000;
const unsigned ROTATED_HEXAGONAL_120_FLAG = 0x10000000;
std::vector<LoadedObject> ObjectLoader::parseObjectLayers(
    const json& mapJson, 
    const std::map<int, std::string>& gidToAssetPathMap
) {
    std::vector<LoadedObject> parsedObjects;

    if (!mapJson.contains("layers")) return parsedObjects;

    for (const auto& layer : mapJson["layers"]) {
        if (layer.value("type", "") != "objectgroup") continue;
        if (!layer.contains("objects")) continue;

        for (const auto& obj : layer["objects"]) {
            if (!obj.contains("gid")) continue; // We only care about visual object templates

            int rawGid = obj["gid"].get<int>();
            int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | 
                                      FLIPPED_DIAGONALLY_FLAG | ROTATED_HEXAGONAL_120_FLAG);
            
            std::string assetPath = "Unknown_Asset";
            
            // Direct, O(log N) flat lookup. No backwards iterating or local ID calculations needed!
            auto it = gidToAssetPathMap.find(cleanGid);
            if (it != gidToAssetPathMap.end()) {
                assetPath = it->second;
            }

            LoadedObject loadedObj;
            loadedObj.objectAssetPath = assetPath;
            loadedObj.xPos = obj.value("x", 0.0f);
            loadedObj.width = obj.value("width", 0.0f);
            loadedObj.height = obj.value("height", 0.0f);
            
            // Match engine positioning from bottom-left to top-left
            loadedObj.yPos = obj.value("y", 0.0f) - loadedObj.height; 

            parsedObjects.push_back(loadedObj);
        }
    }

    return parsedObjects;
}