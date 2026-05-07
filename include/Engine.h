#ifndef ENGINE_H
#define ENGINE_H
#include "WindowManager.hpp"
#include "AssetManager.hpp"
#include "SpriteRenderer.hpp"
#include "InputManager.hpp"
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