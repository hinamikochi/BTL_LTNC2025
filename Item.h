#pragma once
#include <SDL.h>
#include <string>

// Định nghĩa các loại vật phẩm chính
enum ItemType {
    TYPE_UNKNOWN, // Loại mặc định
    TYPE_GOLD,
    TYPE_STONE,
    TYPE_DIAMOND,
    TYPE_GIFT
};

class Item {
public:
    // Các biến cũ
    SDL_Rect rect;
    bool collected;
    bool beingPulled;
    int score;
    float weight;
    std::string textureId;
    ItemType type;

    // ⭐ THÊM VÀO: Các biến mới cho chức năng móc câu
    float hookOffsetX, hookOffsetY; // Vị trí móc so với tâm vật phẩm
    float currentHookAngle;        // Góc của móc lúc va chạm

    // constructor & destructor
    Item(int x, int y, int w, int h, const std::string& tex, int sc, float wt);
    virtual ~Item() = default;

    // ⭐ THÊM VÀO: Hàm mới để ghi lại dữ liệu va chạm
    void setHookData(float hookAngle, float itemCenterX, float itemCenterY, float hookTipX, float hookTipY);

    // hàm vẽ
    virtual void render();
};