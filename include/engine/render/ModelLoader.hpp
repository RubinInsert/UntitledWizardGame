#ifndef MODELLOADER_HPP
#define MODELLOADER_HPP

#include "engine/render/Mesh.hpp"
#include <string>

class ModelLoader {
public:
    static Mesh Load(const std::string& path);
};

#endif