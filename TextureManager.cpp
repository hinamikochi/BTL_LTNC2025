#include "TextureManager.h"
#include "Game.h" 
#include <SDL_image.h>
#include <iostream>

// Khai báo biến static
std::map<std::string, SDL_Texture*> TextureManager::textures;

bool TextureManager::Load(const std::string& id, const std::string& fileName) {
    SDL_Texture* tex = IMG_LoadTexture(Game::renderer, fileName.c_str());
    if (!tex) {
        std::cerr << "Load texture failed: " << fileName << " " << IMG_GetError() << "\n";
        return false;
    }
    textures[id] = tex;
    return true;
}

void TextureManager::Draw(const std::string& id, int x, int y, int w, int h) {
    SDL_Rect dst{ x, y, w, h };
    SDL_Texture* tex = Get(id);
    if (tex) {
        SDL_RenderCopy(Game::renderer, tex, nullptr, &dst);
    }
}

void TextureManager::DrawEx(const std::string& id, int x, int y, int w, int h,
    double angleDeg, const SDL_Rect* srcRect, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect destRect = { x, y, w, h };
    SDL_Texture* tex = Get(id);

    
    if (tex) {
        SDL_RenderCopyEx(Game::renderer, tex, srcRect, &destRect, angleDeg, center, flip);
    }
}

void TextureManager::Clear() {
    for (auto& p : textures) {
        SDL_DestroyTexture(p.second);
    }
    textures.clear();
}

SDL_Texture* TextureManager::Get(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    std::cerr << "TextureManager::Get warning: texture id not found -> " << id << "\n";
    return nullptr;
}