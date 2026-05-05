#ifndef LTEXTURE_H
#define LTEXTURE_H
#include <SDL3/SDL.h>
#include <string>

extern SDL_Renderer* gRenderer;
class LTexture {
    public:
        LTexture();
        ~LTexture();

        // Member functions
        bool loadFromFile(std::string path);
        
        void render(float x, float y);
        void destroy();

        int getWidth() const;
        int getHeight() const;


        bool isLoaded();
    private:
        SDL_Texture* mTexture;
        
        int mWidth;
        int mHeight;

};

#endif