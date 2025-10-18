#include "Hook.h"
#include "TextureManager.h"
#include "Game.h"
#include <cmath>

Hook& Hook::getInstance() {
    static Hook instance;
    return instance;
}

Hook::Hook() {
    ox = Game::SCREEN_W / 2;
    oy = 25 + (128 / 3);
    angle = (float)(PI / 2.0);
    speedOut = 500.0f;
    baseSpeedIn = 250.0f;
    length = 40.0f;
    extending = false;
    retracting = false;
    attached = nullptr;
}

//  THÊM PHẦN THÂN CHO HÀM RESET
void Hook::reset() {
    length = 40.0f;
    angle = (float)(PI / 2.0);
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
        // Thay vì gán trực tiếp, gọi hàm setHookData của item
        float itemCenterX = item->rect.x + item->rect.w / 2.0f;
        float itemCenterY = item->rect.y + item->rect.h / 2.0f;
        float hookTipX = ox + length * std::cos(angle);
        float hookTipY = oy + length * std::sin(angle);
        item->setHookData((float)angle, itemCenterX, itemCenterY, hookTipX, hookTipY);
    }
    extending = false;
    retracting = true;
}

Item* Hook::update(float dt) {
    Item* collectedItem = nullptr;

    if (!extending && !retracting && !attached) {
        static float t = 0.0f;
        t += dt;
        float freq = 1.5f;
        float amplitude = (float)(65 * PI / 180);
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
            // Cập nhật vị trí item dựa trên dữ liệu hook đã lưu
            float itemCenterX = tx - attached->hookOffsetX;
            float itemCenterY = ty - attached->hookOffsetY;
            attached->rect.x = (int)(itemCenterX - attached->rect.w / 2.0f);
            attached->rect.y = (int)(itemCenterY - attached->rect.h / 2.0f);
        }

        if (length <= 40.0f) {
            length = 40.0f;
            retracting = false;
            if (attached) {
                collectedItem = attached;
                attached = nullptr;
            }
        }
    }
    return collectedItem;
}

void Hook::render() {
    int ropeTextureW = 50;
    float renderAngleDeg = (float)(angle * 180.0 / PI - 90.0);
    SDL_Point pivot = { ropeTextureW / 2, 0 };
    TextureManager::DrawEx("rope", (int)(ox - pivot.x), oy, ropeTextureW, (int)length, renderAngleDeg, nullptr, &pivot);

    if (!attached) {
        SDL_Texture* hookTex = TextureManager::Get("hook");
        int hookW = 0, hookH = 0;
        if (hookTex) {
            SDL_QueryTexture(hookTex, nullptr, nullptr, &hookW, &hookH);
        }
        hookW = int(hookW * 0.8f);
        hookH = int(hookH * 0.8f);
        int tx = int(ox + length * std::cos(angle));
        int ty = int(oy + length * std::sin(angle));
        SDL_Point hookCenter = { hookW / 2, hookH / 2 };
        TextureManager::DrawEx("hook", tx - hookW / 2, ty - hookH / 2, hookW, hookH, renderAngleDeg, nullptr, &hookCenter);
    }
}

SDL_Rect Hook::getTipRect() const {
    int tx = int(ox + length * std::cos(angle));
    int ty = int(oy + length * std::sin(angle));
    return SDL_Rect{ tx - 8, ty - 8, 16, 16 };
}