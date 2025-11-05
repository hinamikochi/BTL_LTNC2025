#pragma once
#include "Item.h"
#include "TextureManager.h"
#include <SDL.h>

class Gold : public Item {
public:
    enum Size { SMALL, NORMAL, LARGE };
    Size size;

    Gold(int x, int y, Size sz)
        : Item(0, 0, 0, 0, "", 0, 0.0f)
    {
        type = TYPE_GOLD;
        size = sz;

        SDL_Texture* tex = nullptr;
        int texW = 0, texH = 0;

        switch (size) {
        case SMALL:
            textureId = "gold_small";
            score = 50;
            weight = 0.8f;
            break;
        case NORMAL:
            textureId = "gold_normal";
            score = 100;
            weight = 1.0f;
            break;
        case LARGE:
            textureId = "gold_large";
            score = 250;
            weight = 3.0f;
            break;
        }

        tex = TextureManager::Get(textureId);
        if (tex) SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);
        rect = { x, y, texW, texH };


        if (size == LARGE) {
            rect.w = int(texW * 0.9f);
            rect.h = int(texH * 0.9f);
        }

        collected = false;
        beingPulled = false;
    }
};