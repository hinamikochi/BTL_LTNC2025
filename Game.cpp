#include "Game.h"
#include "TextureManager.h"
#include "Hook.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

SDL_Renderer* Game::renderer = nullptr;
int Game::SCREEN_W = 800;
int Game::SCREEN_H = 600;

// Khởi tạo tất cả các biến
Game::Game() : window(nullptr), isRunning(false), score(0), lastTick(0),
gameState(START_SCREEN), font(nullptr), startTextTexture(nullptr),
isStartTextVisible(true), blinkTimer(0.0f), gStartMusic(nullptr),
mStartSound(nullptr), mStartClickSound(nullptr), gGameplayMusic(nullptr), mClawFireSound(nullptr), mItemHitSound(nullptr), mItemCollectSound(nullptr),
isMuted(false),
mHudFont(nullptr), mLevelGoal(0), mTimeLeft(0.0f) {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    SCREEN_W = width; SCREEN_H = height;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { return false; }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { return false; }
    if (TTF_Init() == -1) { return false; }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { return false; }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) { return false; }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { return false; }

    // Tải tài nguyên cho màn hình khởi động
    gStartMusic = Mix_LoadMUS("assets/StartMusic.ogg");
    if (gStartMusic) Mix_PlayMusic(gStartMusic, -1);

    TextureManager::Load("start_screen", "assets/StartScreen.png");
    font = TTF_OpenFont("assets/arial.ttf", 48);

    SDL_Color textColor = { 255, 255, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, "START", textColor);
    startTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    startTextRect.w = textSurface->w;
    startTextRect.h = textSurface->h;
    SDL_FreeSurface(textSurface);

    int nuggetCenterX = 221;
    int nuggetCenterY = 220;
    startTextRect.x = nuggetCenterX - (startTextRect.w / 2);
    startTextRect.y = nuggetCenterY - (startTextRect.h / 2);

    // Tải tài nguyên cho nút Mute
    TextureManager::Load("sound_on", "assets/sound.png");
    TextureManager::Load("sound_off", "assets/no-sound.png");
    muteButtonRect = { 10, 10, 50, 50 };

    // Tải font và gán giá trị cho HUD
    mHudFont = TTF_OpenFont("assets/arial.ttf", 20); // Cỡ chữ nhỏ hơn
    mLevelGoal = 650;
    mTimeLeft = 60.0f;

    // Tải âm thanh cho gameplay
    gGameplayMusic = Mix_LoadMUS("assets/SoundInGame.mp3");
    mClawFireSound = Mix_LoadWAV("assets/Rope.wav");
    mItemHitSound = Mix_LoadWAV("assets/693582__rottako__drop-item-heavy2.mp3");
    mItemCollectSound = Mix_LoadWAV("assets/GetReward.ogg"); 
    mStartClickSound = Mix_LoadWAV("assets/Click.wav");

    // Tải tài nguyên hình ảnh cho gameplay
    TextureManager::Load("background", "assets/bg.png");
    TextureManager::Load("background2", "assets/bg2.png");
    TextureManager::Load("player", "assets/player.png");
    TextureManager::Load("hook", "assets/moc.png");
    TextureManager::Load("hook_on_item", "assets/hook_on_item.png");
    TextureManager::Load("gold_small", "assets/vang_4.png");
    TextureManager::Load("gold_normal", "assets/vang_3.png");
    TextureManager::Load("gold_large", "assets/vang_1.png");
    TextureManager::Load("stone_small", "assets/da_2.png");
    TextureManager::Load("stone_normal", "assets/da_1.png");
    TextureManager::Load("stone_large", "assets/big_stone_upscaled.png");
    TextureManager::Load("rope", "assets/day1 (4).png");
    TextureManager::Load("diamond", "assets/diamond (1).png");
    TextureManager::Load("gift_bag", "assets/SurpriseBag.png");


    // Tạo các item
    items.push_back(new Gold(600, 400, Gold::NORMAL));
    items.push_back(new Gold(420, 450, Gold::LARGE));
    items.push_back(new Gold(200, 430, Gold::SMALL));
    items.push_back(new Stone(300, 500, Stone::SMALL));
    items.push_back(new Stone(500, 520, Stone::NORMAL));
    items.push_back(new Stone(150, 480, Stone::LARGE));
    items.push_back(new Diamond(300, 400));
    items.push_back(new Gift(500, 380));
    items.push_back(new Gift(100, 420));

    isRunning = true;
    lastTick = SDL_GetTicks();
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mousePoint = { event.button.x, event.button.y };

            // 1. Kiểm tra click vào nút Mute
            if (SDL_PointInRect(&mousePoint, &muteButtonRect)) {
                // <<< PHÁT ÂM THANH CLICK CHO NÚT MUTE >>>
                if (!isMuted) {
                    Mix_PlayChannel(-1, mStartClickSound, 0);
                }

                // Logic Mute
                isMuted = !isMuted;
                if (isMuted) {
                    Mix_VolumeMusic(0);
                }
                else {
                    Mix_VolumeMusic(MIX_MAX_VOLUME);
                }
            }
            // 2. Kiểm tra click vào nút START
            else if (gameState == START_SCREEN && SDL_PointInRect(&mousePoint, &startTextRect)) {
                // <<< PHÁT ÂM THANH CLICK CHO NÚT START >>>
                if (!isMuted) {
                    Mix_PlayChannel(-1, mStartClickSound, 0);
                }

                // Logic chuyển màn và đổi nhạc
                gameState = PLAYING;
                Mix_HaltMusic();
                Mix_PlayMusic(gGameplayMusic, -1);
            }
        }

        if (event.type == SDL_KEYDOWN) {
            if (gameState == PLAYING) {
                if (event.key.keysym.sym == SDLK_ESCAPE) isRunning = false;
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (!isMuted) Mix_PlayChannel(-1, mClawFireSound, 0);
                    Hook::getInstance().fire();
                }
            }
        }
    }
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    float dt = (now - lastTick) / 1000.0f;
    lastTick = now;

    if (gameState == START_SCREEN) {
        blinkTimer += dt;
        if (blinkTimer >= 0.5f) {
            isStartTextVisible = !isStartTextVisible;
            blinkTimer = 0.0f;
        }
    }
    else if (gameState == PLAYING) {
        if (mTimeLeft > 0) mTimeLeft -= dt; else mTimeLeft = 0;

        // <<< LOGIC MỚI ĐỂ XỬ LÝ THU THẬP VÀ VA CHẠM >>>

        // 1. Cập nhật Hook và nhận về vật phẩm đã thu thập (nếu có)
        Item* collectedItem = Hook::getInstance().update(dt);

        // 2. Nếu có vật phẩm vừa được thu thập
        if (collectedItem != nullptr) {
            if (!isMuted) Mix_PlayChannel(-1, mItemCollectSound, 0); // Phát âm thanh "Ting"
            score += collectedItem->score; // Cộng điểm

            // Tìm và xóa vật phẩm khỏi danh sách
            for (size_t i = 0; i < items.size(); ++i) {
                if (items[i] == collectedItem) {
                    delete items[i];
                    items.erase(items.begin() + i);
                    break;
                }
            }
        }

        // 3. Xử lý va chạm chỉ khi móc câu đang đi ra
        if (Hook::getInstance().isExtending()) {
            SDL_Rect hookRect = Hook::getInstance().getTipRect();
            for (auto& it : items) {
                if (!it->beingPulled && SDL_HasIntersection(&hookRect, &it->rect)) {
                    if (!isMuted) Mix_PlayChannel(-1, mItemHitSound, 0); // Phát âm thanh va chạm
                    Hook::getInstance().attach(it);
                    it->beingPulled = true;
                    // Không cộng điểm ở đây nữa
                    break;
                }
            }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    if (gameState == START_SCREEN) {
        TextureManager::Draw("start_screen", 0, 0, SCREEN_W, SCREEN_H);
        if (isStartTextVisible) {
            SDL_RenderCopy(renderer, startTextTexture, NULL, &startTextRect);
        }
    }
    else if (gameState == PLAYING) {
        // Vẽ gameplay
        TextureManager::Draw("background", 0, 0, SCREEN_W, SCREEN_H);
        TextureManager::Draw("background2", 0, 5, SCREEN_W, 90);

        float originalPlayerW = 212.0f;
        float originalPlayerH = 133.0f;
        int playerW = 150;
        int playerH = (int)(playerW * (originalPlayerH / originalPlayerW));
        int playerX = (int)(SCREEN_W / 2.0f - playerW / 2.0f);
        int playerY = 5;
        TextureManager::Draw("player", playerX, playerY, playerW, playerH);

        for (auto& it : items) {
            it->render();
        }
        Hook::getInstance().render();

        // <<< HUD ĐÃ SỬA ĐỔI >>>
        SDL_Color green = { 0, 255, 0, 255 };
        SDL_Color red = { 255, 0, 0, 255 };

        // --- Vẽ Tiền (Score) ---
        std::string scoreText = "Tien: $" + std::to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Blended(mHudFont, scoreText.c_str(), green);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = { 80, 25, scoreSurface->w, scoreSurface->h }; // Lui sang phải
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);

        // --- Vẽ Thời gian (Time) ---
        std::string timeText = "TG: " + std::to_string(static_cast<int>(mTimeLeft));
        SDL_Surface* timeSurface = TTF_RenderText_Blended(mHudFont, timeText.c_str(), red);
        SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_Rect timeRect = { 700, 25, timeSurface->w, timeSurface->h };
        SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);
        SDL_FreeSurface(timeSurface);
        SDL_DestroyTexture(timeTexture);
    }

    // Vẽ nút Mute
    if (isMuted) {
        TextureManager::Draw("sound_off", muteButtonRect.x, muteButtonRect.y, muteButtonRect.w, muteButtonRect.h);
    }
    else {
        TextureManager::Draw("sound_on", muteButtonRect.x, muteButtonRect.y, muteButtonRect.w, muteButtonRect.h);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TextureManager::Clear();

    if (startTextTexture) SDL_DestroyTexture(startTextTexture);
    if (font) TTF_CloseFont(font);
    if (mHudFont) TTF_CloseFont(mHudFont);

    if (gStartMusic) Mix_FreeMusic(gStartMusic);
    if (mStartClickSound) Mix_FreeChunk(mStartClickSound);
    if (mStartSound) Mix_FreeChunk(mStartSound);
    if (gGameplayMusic) Mix_FreeMusic(gGameplayMusic);
    if (mClawFireSound) Mix_FreeChunk(mClawFireSound);
    if (mItemHitSound) Mix_FreeChunk(mItemHitSound);
    if (mItemCollectSound) Mix_FreeChunk(mItemCollectSound);

    for (auto& it : items) {
        delete it;
    }
    items.clear();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}