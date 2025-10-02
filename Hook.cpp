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

void Hook::update(float dt) {
    // Không cần cập nhật ox ở đây nữa vì nó đã được khởi tạo và không đổi
    // ox = Game::SCREEN_W / 2;

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

        if (tx < 0 || tx > Game::SCREEN_W || ty > Game::SCREEN_H || length > 800) { // Tăng giới hạn chiều dài
            extending = false;
            retracting = true;
        }
    }
    else if (retracting) {
        float spd = baseSpeedIn;
        if (attached) spd = baseSpeedIn / attached->weight;

        length -= spd * dt;

        // ⭐ BẮT ĐẦU SỬA LỖI: Cập nhật vị trí vật phẩm
        if (attached) {
            // 1. Tính toán vị trí của đầu sợi dây (rope tip)
            int tx = (int)(ox + length * std::cos(angle));
            int ty = (int)(oy + length * std::sin(angle));

            // 2. Cập nhật vị trí của vật phẩm
            // để "điểm bị móc trúng" trên vật phẩm di chuyển theo đầu dây
            attached->rect.x = (int)(tx - attached->rect.w / 2.0f - attached->hookOffsetX);
            attached->rect.y = (int)(ty - attached->rect.h / 2.0f - attached->hookOffsetY);
        }
        // ⭐ KẾT THÚC SỬA LỖI

        if (length <= 40.0f) {
            length = 40.0f;
            retracting = false;
            if (attached) {
                attached->collected = true;
                attached->beingPulled = false;
                attached = nullptr;
            }
        }
    }
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