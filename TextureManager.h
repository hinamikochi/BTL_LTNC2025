#pragma once
#include <string>
#include <map>
#include <SDL.h>

class TextureManager {
public:
    static std::map<std::string, SDL_Texture*> textures;
    static bool Load(const std::string& id, const std::string& fileName);
    static void Draw(const std::string& id, int x, int y, int w, int h);

   
    static void DrawEx(const std::string& id, int x, int y, int w, int h,
        double angleDeg = 0.0, const SDL_Rect* srcRect = nullptr,  
        SDL_Point* center = nullptr,
        SDL_RendererFlip flip = SDL_FLIP_NONE);

    static void Clear();
    static SDL_Texture* Get(const std::string& id);
};