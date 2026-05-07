#ifndef ENGINE_H
#define ENGINE_H
#include "engine/core/WindowManager.hpp"
#include "engine/core/AssetManager.hpp"
#include "engine/render/SpriteRenderer.hpp"
#include "engine/core/InputManager.hpp"
#include <entt/entt.hpp>
class Engine {
    public:
        Engine();
        bool Init();
        int Run();
    
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        InputManager inputManager;
        entt::registry registry;
        SpriteRenderer spriteRenderer;
        Camera camera;
};

#endif