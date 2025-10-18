#pragma once
#include "Item.h"

class Stone : public Item {
public:
    enum Size { SMALL, NORMAL, LARGE };
    Size size; 

    Stone(int x, int y, Size sz)
        : Item(0, 0, 0, 0, "", 0, 0.0f) 
    {
        type = TYPE_STONE; 
        size = sz;         

        switch (size) {
        case SMALL:
            rect = { x, y, 32, 32 };
            score = 20;
            weight = 1.2f;
            textureId = "stone_small";
            break;
        case NORMAL:
            rect = { x, y, 48, 48 };
            score = 35;
            weight = 2.0f;
            textureId = "stone_normal";
            break;
        case LARGE:
            rect = { x, y, 70, 70 };
            score = 50;
            weight = 3.0f;
            textureId = "stone_large";
            break;
        }
        collected = false;
        beingPulled = false;
    }
};