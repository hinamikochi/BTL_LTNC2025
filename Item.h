#pragma once
#include <SDL.h>
#include <string>
#include "TextureManager.h"

// Định nghĩa các loại vật phẩm chính
enum ItemType {
    TYPE_UNKNOWN, 
    TYPE_GOLD,
    TYPE_STONE,
    TYPE_DIAMOND,
    TYPE_GIFT
};

class Item {
public:
    // Các biến của bạn
    SDL_Rect rect;
    bool collected;
    bool beingPulled;
    int score;
    float weight;
    std::string textureId;
    ItemType type;
    float hookOffsetX, hookOffsetY;
    float currentHookAngle;

    // constructor & destructor
    Item(int x, int y, int w, int h, const std::string& tex, int sc, float wt);
    virtual ~Item() = default;

    // Hàm của bạn
    void setHookData(float hookAngle, float itemCenterX, float itemCenterY, float hookTipX, float hookTipY);
    virtual void render();

    
    const SDL_Rect& getRect() const;
    int getValue() const;
    void setPosition(int x, int y);
    bool isCaught() const;
    void setCaught(bool isCaught);
};