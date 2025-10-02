#include "Game.h"
#include "TextureManager.h"
#include "Hook.h"
#include <SDL_image.h>
#include <iostream>

SDL_Renderer* Game::renderer = nullptr;
int Game::SCREEN_W = 800;
int Game::SCREEN_H = 600;

Game::Game() : window(nullptr), isRunning(false), score(0), lastTick(0) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int width, int height) {
    SCREEN_W = width; SCREEN_H = height;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init: " << IMG_GetError() << "\n";
        return false;
    }
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) {
        std::cerr << "Window error: " << SDL_GetError() << "\n";
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Load textures
    TextureManager::Load("background", "assets/bg.png");
    TextureManager::Load("background2", "assets/bg2.png");
    TextureManager::Load("player", "assets/player.png");
    TextureManager::Load("hook", "assets/moc.png");
    TextureManager::Load("hook_on_item", "assets/hook_on_item.png");

    // gold images
    TextureManager::Load("gold_small", "assets/vang_4.png");
    TextureManager::Load("gold_normal", "assets/vang_3.png");
    TextureManager::Load("gold_large", "assets/vang_1.png");

    // stone images
    TextureManager::Load("stone_small", "assets/da_2.png");
    TextureManager::Load("stone_normal", "assets/da_1.png");
    TextureManager::Load("stone_large", "assets/big_stone_upscaled.png");
    
    // rope images
    TextureManager::Load("rope", "assets/day1 (4).png");

    // tạo golds
    items.push_back(new Gold(600, 400, Gold::NORMAL));
    items.push_back(new Gold(420, 450, Gold::LARGE));
    items.push_back(new Gold(200, 430, Gold::SMALL));

    // tạo stones
    items.push_back(new Stone(300, 500, Stone::SMALL));
    items.push_back(new Stone(500, 520, Stone::NORMAL));
    items.push_back(new Stone(150, 480, Stone::LARGE));

    isRunning = true;
    lastTick = SDL_GetTicks();
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) isRunning = false;
            if (event.key.keysym.sym == SDLK_SPACE) {
                Hook::getInstance().fire();
            }
        }
    }
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    float dt = (now - lastTick) / 1000.0f;
    lastTick = now;
    Hook::getInstance().update(dt);

    SDL_Rect hookRect = Hook::getInstance().getTipRect();

    // check collision với mọi item
    for (auto& it : items) {
        if (!it->collected && !it->beingPulled && SDL_HasIntersection(&hookRect, &it->rect)) {
            it->beingPulled = true;              // đánh dấu item đang bị kéo
            Hook::getInstance().attach(it);      // gắn vào hook
            score += it->score;
            float itemCenterX = it->rect.x + it->rect.w / 2.0f;
            float itemCenterY = it->rect.y + it->rect.h / 2.0f;
            float hookTipX = hookRect.x + hookRect.w / 2.0f;
            float hookTipY = hookRect.y + hookRect.h / 2.0f;
            it->setHookData(Hook::getInstance().getAngle(), itemCenterX, itemCenterY, hookTipX, hookTipY);
            break;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    // background
    TextureManager::Draw("background", 0, 0, SCREEN_W, SCREEN_H);
    TextureManager::Draw("background2", 0, 5, SCREEN_W, 90);

    // =================== PHẦN SỬA LỖI TỶ LỆ ẢNH ===================

    // 1. Điền kích thước GỐC của file player.png (212 x 133)
    float originalPlayerW = 212.0f;
    float originalPlayerH = 133.0f;

    // 2. Đặt chiều rộng mong muốn khi vẽ ra màn hình.
    //    Bạn có thể thay đổi số 150 này để nhân vật to/nhỏ tùy ý.
    int playerW = 150;

    // 3. Tự động tính chiều cao để giữ đúng tỷ lệ, không bị méo hình.
    int playerH = (int)(playerW * (originalPlayerH / originalPlayerW));

    // 4. Căn chỉnh vị trí người thợ mỏ
    int playerX = (int)(SCREEN_W / 2.0f - playerW / 2.0f);
    int playerY = 5; // Giữ nguyên giá trị y=3 bạn đã tinh chỉnh thành công

    // 5. Lệnh vẽ người thợ mỏ với kích thước đã được tính toán lại
    TextureManager::Draw("player", playerX, playerY, playerW, playerH);

    // ===============================================================

    // vẽ tất cả item
    for (auto& it : items) {
        it->render();
    }

    // vẽ hook
    Hook::getInstance().render();

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TextureManager::Clear();

    // free items
    for (auto& it : items) {
        delete it;
    }
    items.clear();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
