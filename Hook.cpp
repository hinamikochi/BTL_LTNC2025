#include "Hook.h"
#include "TextureManager.h"
#include "Game.h"
#include <cmath>

Hook& Hook::getInstance() {
    static Hook instance;
    return instance;
}

Hook::Hook() {
    ox = Game::SCREEN_W / 2.0f;
    oy = 25 + (128 / 3.0f); // Sửa lại theo playerY = 3
    angle = (float)(PI / 2.0);
    speedOut = 500.0f;
    baseSpeedIn = 250.0f;
    length = 40.0f;
    extending = false;
    retracting = false;
    attached = nullptr;
}

void Hook::fire() {
    if (!extending && !retracting) {
        extending = true;
    }
}

void Hook::attach(Item* item) {
    attached = item;
    if (attached) {
        attached->beingPulled = true;
    }
    extending = false;
    retracting = true;
}

Item* Hook::update(float dt) {
    Item* collectedItem = nullptr; // Biến để lưu vật phẩm vừa thu thập được

    if (!extending && !retracting && !attached) {
        // idle: quay qua lại
        static float t = 0.0f;
        t += dt;
        float freq = 1.0f;
        float amplitude = (float)(PI / 2.0 - 0.05);
        angle = (float)(PI / 2.0 + amplitude * std::sin(t * freq));
    }
    else if (extending) {
        length += speedOut * dt;
        int tx = int(ox + length * std::cos(angle));
        int ty = int(oy + length * std::sin(angle));

        if (tx < 0 || tx > Game::SCREEN_W || ty > Game::SCREEN_H || length > 800) {
            extending = false;
            retracting = true;
        }
    }
    else if (retracting) {
        float spd = baseSpeedIn;
        if (attached) spd = baseSpeedIn / attached->weight;

        length -= spd * dt;

        if (attached) {
            int tx = (int)(ox + length * std::cos(angle));
            int ty = (int)(oy + length * std::sin(angle));
            attached->rect.x = (int)(tx - attached->rect.w / 2.0f - attached->hookOffsetX);
            attached->rect.y = (int)(ty - attached->rect.h / 2.0f - attached->hookOffsetY);
        }

        if (length <= 40.0f) {
            length = 40.0f;
            retracting = false;
            if (attached) {
                // Thay vì chỉ xóa, chúng ta gán nó vào biến để trả về
                collectedItem = attached;
                attached = nullptr;
            }
        }
    }

    return collectedItem; // Trả về vật phẩm đã thu thập (hoặc nullptr nếu không có)
}

void Hook::render() {
    // Vẽ dây câu (giữ nguyên)
    int ropeTextureW = 50;
    int ropeTextureH = 64;
    float renderAngleDeg = (float)(angle * 180.0 / PI - 90.0);
    SDL_Point pivot = { ropeTextureW / 2, 0 };
    int render_x = (int)(ox - pivot.x + 0.5f);
    int render_y = (int)(oy - pivot.y + 0.5f);
    TextureManager::DrawEx("rope", render_x, render_y, ropeTextureW, (int)length, renderAngleDeg, nullptr, &pivot, SDL_FLIP_NONE);

    // Chỉ vẽ móc câu chính khi nó chưa gắp được gì
    if (!attached) {
        // ⭐ BẮT ĐẦU SỬA: Lấy kích thước thật của ảnh móc câu
        // 1. Lấy texture của móc câu
        SDL_Texture* hookTex = TextureManager::Get("hook");
        int hookW = 0, hookH = 0;

        // 2. Lấy kích thước thật của texture đó
        if (hookTex) {
            SDL_QueryTexture(hookTex, nullptr, nullptr, &hookW, &hookH);
        }

        hookW *= 0.8f;
        hookH *= 0.8f;

        // 3. Tính toán vị trí và vẽ với kích thước thật
        int tx = int(ox + length * std::cos(angle));
        int ty = int(oy + length * std::sin(angle));
        SDL_Point hookCenter = { hookW / 2, hookH / 2 };

        TextureManager::DrawEx("hook", tx - hookW / 2, ty - hookH / 2, hookW, hookH, renderAngleDeg, nullptr, &hookCenter, SDL_FLIP_NONE);
        // ⭐ KẾT THÚC SỬA
    }
}

SDL_Rect Hook::getTipRect() const {
    int tx = int(ox + length * std::cos(angle));
    int ty = int(oy + length * std::sin(angle));
    return SDL_Rect{ tx - 8, ty - 8, 16, 16 };
}