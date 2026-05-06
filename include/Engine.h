#ifndef ENGINE_H
#define ENGINE_H
#include "WindowManager.hpp"
#include "AssetManager.hpp"
#include "SpriteRenderer.hpp"
class Engine {
    public:
        Engine();
        bool Init();
        int Run();
    
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        SpriteRenderer spriteRenderer;
};
#endif