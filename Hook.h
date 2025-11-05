#pragma once
#include <SDL.h>
#include "Item.h"

static constexpr double PI = 3.14159265358979323846;

class Hook {
public:
    static Hook& getInstance();
    Item* update(float dt);
    void render();
    void fire();
    SDL_Rect getTipRect() const;
    void attach(Item* item);
    float getAngle() const { return angle; }
    bool isExtending() const { return extending; }


    void reset();

private:
    Hook();
    float angle;
    float length;
    int ox, oy;
    float speedOut;
    float baseSpeedIn;
    bool extending;
    bool retracting;
    Item* attached;
};