#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <SDL3/SDL.h>
#include <string>
class WindowManager {
    public:
        WindowManager();
        ~WindowManager();
        bool createWindow(std::string windowTitle, int windowWidth, int windowHeight);
        SDL_Window* getWindow();
        SDL_Renderer* getRenderer();
        void destroyAllWindows();

    private:
        // The window to be rendered to
        SDL_Window* gameWindow{ nullptr };

        // The surface contained by the window
        SDL_Renderer* gameRenderer{ nullptr };
};
#endif