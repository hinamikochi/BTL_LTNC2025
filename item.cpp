#include "Item.h"

#include "TextureManager.h"

#include "Game.h"

#include "Hook.h" // ⭐ THÊM VÀO: Cần để lấy hằng số PI



Item::Item(int x, int y, int w, int h, const std::string& tex, int sc, float wt)

{

	rect = { x, y, w, h };

	textureId = tex;

	score = sc;

	weight = wt;

	collected = false;

	beingPulled = false;

	type = TYPE_UNKNOWN; // Khởi tạo giá trị mặc định

	hookOffsetX = 0;

	hookOffsetY = 0;

	currentHookAngle = 0;

}



// ⭐ THÊM VÀO: Định nghĩa cho hàm mới

void Item::setHookData(float hookAngle, float itemCenterX, float itemCenterY, float hookTipX, float hookTipY) {

	currentHookAngle = hookAngle;

	hookOffsetX = hookTipX - itemCenterX;

	hookOffsetY = hookTipY - itemCenterY;

}



// ⭐ SỬA LẠI HOÀN TOÀN HÀM RENDER

// Trong Item.cpp
void Item::render() {
    // Chỉ không vẽ khi đã được thu thập hoàn toàn
    if (collected) return;

    // 1. Luôn luôn vẽ vật phẩm
    TextureManager::Draw(textureId, rect.x, rect.y, rect.w, rect.h);

    // 2. NẾU ĐANG BỊ KÉO, VẼ THÊM CÁI MÓC TRANG TRÍ CHUNG LÊN TRÊN
    if (beingPulled) {
        // Lấy kích thước thật của móc trang trí để không bị co
        SDL_Texture* hookTex = TextureManager::Get("hook_on_item");
        int hookW = 0, hookH = 0;
        if (hookTex) {
            SDL_QueryTexture(hookTex, nullptr, nullptr, &hookW, &hookH);
        }

        hookW *= 0.8f;
        hookH *= 0.8f;

        // Tính toán vị trí và vẽ móc trang trí
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
