#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <map>
struct LoadedObject {
    std::string objectAssetPath;
    float xPos;
    float yPos;
    float width;
    float height;
    // float rotation;
    // float opacity;
    // float rotation;
};
class ObjectLoader {
public:
    // Pure, simple data delivery: just pass the raw json and a flat gid -> path map
    static std::vector<LoadedObject> parseObjectLayers(
        const nlohmann::json& mapJson, 
        const std::map<int, std::string>& gidToAssetPathMap
    );
};