#include "Item.h"
#include "TextureManager.h"
#include "Game.h"
#include "Hook.h" 

Item::Item(int x, int y, int w, int h, const std::string& tex, int sc, float wt)
{
    rect = { x, y, w, h };
    textureId = tex;
    score = sc;
    weight = wt;
    collected = false;
    beingPulled = false;
    type = TYPE_UNKNOWN;
    hookOffsetX = 0;
    hookOffsetY = 0;
    currentHookAngle = 0;
}

void Item::setHookData(float hookAngle, float itemCenterX, float itemCenterY, float hookTipX, float hookTipY) {
    currentHookAngle = hookAngle;
    hookOffsetX = hookTipX - itemCenterX;
    hookOffsetY = hookTipY - itemCenterY;
}

void Item::render() {
    if (collected) return;
    TextureManager::Draw(textureId, rect.x, rect.y, rect.w, rect.h);

    if (beingPulled) {
        SDL_Texture* hookTex = TextureManager::Get("hook_on_item");
        int hookW = 0, hookH = 0;
        if (hookTex) {
            SDL_QueryTexture(hookTex, nullptr, nullptr, &hookW, &hookH);
        }
        hookW = int(hookW * 0.8f);
        hookH = int(hookH * 0.8f);
        float itemCenterX = rect.x + rect.w / 2.0f;
        float itemCenterY = rect.y + rect.h / 2.0f;
        float hookRenderX = itemCenterX + hookOffsetX;
        float hookRenderY = itemCenterY + hookOffsetY;
        SDL_Point hookCenter = { hookW / 2, hookH / 2 };
        TextureManager::DrawEx(
            "hook_on_item",
            (int)(hookRenderX - hookW / 2),
            (int)(hookRenderY - hookH / 2),
            hookW, hookH,
            currentHookAngle * 180.0 / PI - 90.0,
            nullptr, &hookCenter
        );
    }
}

const SDL_Rect& Item::getRect() const {
    return rect;
}

int Item::getValue() const {
    return score;
}

void Item::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

bool Item::isCaught() const {
    return beingPulled;
}

void Item::setCaught(bool isCaught) {
    beingPulled = isCaught;
}