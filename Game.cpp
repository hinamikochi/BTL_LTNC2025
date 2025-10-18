#include "Game.h"
#include "TextureManager.h"
#include "Hook.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <random>
#include <algorithm> 

SDL_Renderer* Game::renderer = nullptr;
int Game::SCREEN_W = 800;
int Game::SCREEN_H = 600;

int getRandom(int min, int max) {
    if (min > max) std::swap(min, max);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

Game::Game() : window(nullptr), isRunning(false), score(0), lastTick(0),
gameState(START_SCREEN), font(nullptr), startTextTexture(nullptr),
isStartTextVisible(true), blinkTimer(0.0f), gStartMusic(nullptr),
gGameplayMusic(nullptr), mClawFireSound(nullptr), mItemHitSound(nullptr), mItemCollectSound(nullptr), mStartClickSound(nullptr),
isMuted(false), mHudFont(nullptr), mLevelGoal(0), mTimeLeft(0.0f),
mCurrentLevel(1), mStateTimer(0.0f), mLevelPassTexture(nullptr), mLevelFailTexture(nullptr)
{}

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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("assets/arial.ttf", 48);
    mHudFont = TTF_OpenFont("assets/arial.ttf", 20);

    gStartMusic = Mix_LoadMUS("assets/StartMusic.ogg");
    if (gStartMusic) Mix_PlayMusic(gStartMusic, -1);

    TextureManager::Load("start_screen", "assets/StartScreen.png");
    SDL_Color textColor = { 255, 255, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, "START", textColor);
    startTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    startTextRect = { 221 - textSurface->w / 2, 220 - textSurface->h / 2, textSurface->w, textSurface->h };
    SDL_FreeSurface(textSurface);

    TextureManager::Load("level_board", "assets/waitScreen.png");
    TextureManager::Load("winner", "assets/GameOverBack.png");
    TextureManager::Load("game_over", "assets/gameover.png");

    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Surface* passSurface = TTF_RenderText_Blended(font, "Ban da vuot qua!", black);
    mLevelPassTexture = SDL_CreateTextureFromSurface(renderer, passSurface);
    mLevelPassRect = { SCREEN_W / 2 - passSurface->w / 2, SCREEN_H / 2 - passSurface->h / 2, passSurface->w, passSurface->h };
    SDL_FreeSurface(passSurface);

    SDL_Surface* failSurface = TTF_RenderText_Blended(font, "Ban da thua cuoc", black);
    mLevelFailTexture = SDL_CreateTextureFromSurface(renderer, failSurface);
    mLevelFailRect = { SCREEN_W / 2 - failSurface->w / 2, SCREEN_H / 2 - failSurface->h / 2, failSurface->w, failSurface->h };
    SDL_FreeSurface(failSurface);

    TextureManager::Load("sound_on", "assets/sound.png");
    TextureManager::Load("sound_off", "assets/no-sound.png");
    muteButtonRect = { 10, 10, 50, 50 };

    TextureManager::Load("pause_button", "assets/PauseButton.png");
    TextureManager::Load("resume_button", "assets/ResumeButton.png");
    TextureManager::Load("main_menu_button", "assets/HomeButton.png");

    pauseButtonRect = { 630, 10, 50, 50 };
    int menuW = 300, menuH = 200;
    pauseMenuRect = { SCREEN_W / 2 - menuW / 2, SCREEN_H / 2 - menuH / 2, menuW, menuH };
    int btnW = 100, btnH = 50;
    resumeButtonRect = { pauseMenuRect.x + menuW / 2 - btnW - 10, pauseMenuRect.y + menuH / 2 - btnH / 2, btnW, btnH };
    mainMenuButtonRect = { pauseMenuRect.x + menuW / 2 + 10, pauseMenuRect.y + menuH / 2 - btnH / 2, btnW, btnH };

    gGameplayMusic = Mix_LoadMUS("assets/SoundInGame.mp3");
    mClawFireSound = Mix_LoadWAV("assets/Rope.wav");
    mItemHitSound = Mix_LoadWAV("assets/693582__rottako__drop-item-heavy2.mp3");
    mItemCollectSound = Mix_LoadWAV("assets/GetReward.ogg");
    mStartClickSound = Mix_LoadWAV("assets/Click.wav");

    TextureManager::Load("background", "assets/bg.png");
    TextureManager::Load("background2", "assets/bg2.png");
    TextureManager::Load("player", "assets/player.png");
    TextureManager::Load("rope", "assets/day1 (4).png");
    TextureManager::Load("hook", "assets/moc.png");
    TextureManager::Load("hook_on_item", "assets/hook_on_item.png");
    TextureManager::Load("gold_small", "assets/vang_4.png");
    TextureManager::Load("gold_normal", "assets/vang_3.png");
    TextureManager::Load("gold_large", "assets/vang_1.png");
    TextureManager::Load("stone_small", "assets/da_2.png");
    TextureManager::Load("stone_normal", "assets/normal.png");
    TextureManager::Load("stone_large", "assets/BigStone.png");
    TextureManager::Load("diamond", "assets/diamond (1).png");
    TextureManager::Load("gift_bag", "assets/SurpriseBag.png");

    //------------------------Goal-time---GS----GN------GB----SS------SN----SB-----D------G--//
    mLevelConfigs.push_back({ 1200, 60,  4, 6,  2, 3,  1, 2,  2, 4,  1, 2,  1,2,  0, 0,  1, 1 });  
    mLevelConfigs.push_back({ 2300, 60,  5, 7,  3, 4,  2, 3,  3, 5,  2, 3,  2,3,  0, 0,  1, 2 });  
    mLevelConfigs.push_back({ 3500, 55,  6, 8,  3, 5,  2, 3,  3, 4,  2, 4,  3,4,  1, 3,  1, 2 });  
    mLevelConfigs.push_back({ 4500, 55,  5, 9,  4, 6,  3, 4,  4, 6,  3, 5,  3,4,  2, 4,  2, 3 });  
    mLevelConfigs.push_back({ 5500, 50,  6, 10, 5, 7,  3, 5,  5, 7,  4, 7,  3,4,  3, 5,  2, 3 }); 

    isRunning = true;
    lastTick = SDL_GetTicks();
    return true;
}


void Game::generateItemsForLevel(int level) {
    for (auto& it : items) delete it;
    items.clear();

    if (level > mLevelConfigs.size()) return;
    LevelData config = mLevelConfigs[level - 1];

    const int SPAWN_AREA_TOP = 250, SPAWN_AREA_LEFT = 50;
    const int SPAWN_AREA_RIGHT = SCREEN_W - 50, SPAWN_AREA_BOTTOM = SCREEN_H - 50;

    auto addItem = [&](Item* newItem) {
        bool overlap;
        int max_tries = 20;
        do {
            overlap = false;
            int x = getRandom(SPAWN_AREA_LEFT, SPAWN_AREA_RIGHT - newItem->getRect().w);
            int y = getRandom(SPAWN_AREA_TOP, SPAWN_AREA_BOTTOM - newItem->getRect().h);
            newItem->setPosition(x, y);
            for (const auto& existingItem : items) {
                if (SDL_HasIntersection(&newItem->getRect(), &existingItem->getRect())) {
                    overlap = true;
                    break;
                }
            }
            max_tries--;
        } while (overlap && max_tries > 0);

        if (!overlap) items.push_back(newItem);
        else delete newItem;
        };

    // Tạo vật phẩm với số lượng ngẫu nhiên dựa trên config min/max
    int numGoldSmall = getRandom(config.minGoldSmall, config.maxGoldSmall);
    for (int i = 0; i < numGoldSmall; ++i) addItem(new Gold(0, 0, Gold::SMALL));

    int numGoldNormal = getRandom(config.minGoldNormal, config.maxGoldNormal);
    for (int i = 0; i < numGoldNormal; ++i) addItem(new Gold(0, 0, Gold::NORMAL));

    int numGoldLarge = getRandom(config.minGoldLarge, config.maxGoldLarge);
    for (int i = 0; i < numGoldLarge; ++i) addItem(new Gold(0, 0, Gold::LARGE));

    int numStoneSmall = getRandom(config.minStoneSmall, config.maxStoneSmall);
    for (int i = 0; i < numStoneSmall; ++i) addItem(new Stone(0, 0, Stone::SMALL));

    int numStoneNormal = getRandom(config.minStoneNormal, config.maxStoneNormal);
    for (int i = 0; i < numStoneNormal; ++i) addItem(new Stone(0, 0, Stone::NORMAL));

    int numStoneLarge = getRandom(config.minStoneLarge, config.maxStoneLarge);
    for (int i = 0; i < numStoneLarge; ++i) addItem(new Stone(0, 0, Stone::LARGE));

    int numGift = getRandom(config.minGift, config.maxGift);
    for (int i = 0; i < numGift; ++i) addItem(new Gift(0, 0));


    if (mCurrentLevel >= 3) {
        int numDiamond = getRandom(config.minDiamond, config.maxDiamond);
        for (int i = 0; i < numDiamond; ++i) addItem(new Diamond(0, 0));
    }
}

void Game::resetForNewLevel(int level) {
    mCurrentLevel = level;
    if (level > mLevelConfigs.size()) {
        gameState = GAME_WON;
        return;
    }
    mLevelGoal = mLevelConfigs[level - 1].goal;
    mTimeLeft = mLevelConfigs[level - 1].time;
    Hook::getInstance().reset();
    generateItemsForLevel(level);

    gameState = LEVEL_START;
    mStateTimer = 2.5f;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            isRunning = false;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mousePoint = { event.button.x, event.button.y };
            if (gameState != GAME_WON && gameState != GAME_OVER && SDL_PointInRect(&mousePoint, &muteButtonRect)) {
                if (!isMuted) Mix_PlayChannel(-1, mStartClickSound, 0);
                isMuted = !isMuted;
                if (isMuted) Mix_VolumeMusic(0); else Mix_VolumeMusic(MIX_MAX_VOLUME);
            }
            else {
                switch (gameState) {
                case START_SCREEN:
                    if (SDL_PointInRect(&mousePoint, &startTextRect)) {
                        if (!isMuted) Mix_PlayChannel(-1, mStartClickSound, 0);
                        score = 0;
                        resetForNewLevel(1);
                        Mix_HaltMusic();
                        Mix_PlayMusic(gGameplayMusic, -1);
                    }
                    break;
                case PLAYING:
                    if (SDL_PointInRect(&mousePoint, &pauseButtonRect)) {
                        if (!isMuted) Mix_PlayChannel(-1, mStartClickSound, 0);
                        gameState = PAUSED;
                        Mix_PauseMusic();
                    }
                    break;
                case PAUSED:
                    if (SDL_PointInRect(&mousePoint, &resumeButtonRect)) {
                        if (!isMuted) Mix_PlayChannel(-1, mStartClickSound, 0);
                        gameState = PLAYING;
                        Mix_ResumeMusic();
                    }
                    else if (SDL_PointInRect(&mousePoint, &mainMenuButtonRect)) {
                        if (!isMuted) Mix_PlayChannel(-1, mStartClickSound, 0);
                        gameState = START_SCREEN;
                        Mix_HaltMusic();
                        Mix_PlayMusic(gStartMusic, -1);
                        Hook::getInstance().reset();
                        for (auto& item : items) delete item;
                        items.clear();
                    }
                    break;
                case LEVEL_END:
                    if (score >= mLevelGoal) {
                        if (mCurrentLevel >= mLevelConfigs.size()) gameState = GAME_WON;
                        else resetForNewLevel(mCurrentLevel + 1);
                    }
                    else {
                        gameState = GAME_OVER;
                    }
                    break;
                case GAME_WON:
                case GAME_OVER:
                    mCurrentLevel = 1;
                    gameState = START_SCREEN;
                    Mix_HaltMusic();
                    Mix_PlayMusic(gStartMusic, -1);
                    break;
                }
            }
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            switch (gameState) {
            case PLAYING:
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (!isMuted) Mix_PlayChannel(-1, mClawFireSound, 0);
                    Hook::getInstance().fire();
                }
                break;
            default:
                break;
            }
        }
    }
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    float dt = (now - lastTick) / 1000.0f;
    lastTick = now;

    switch (gameState) {
    case START_SCREEN:
        blinkTimer += dt;
        if (blinkTimer >= 0.5f) {
            isStartTextVisible = !isStartTextVisible;
            blinkTimer = 0.0f;
        }
        break;
    case LEVEL_START:
        mStateTimer -= dt;
        if (mStateTimer <= 0) {
            gameState = PLAYING;
            if (Mix_PausedMusic() == 1) {
                Mix_ResumeMusic();
            }
        }
        break;
    case PLAYING: {
        if (mTimeLeft > 0) mTimeLeft -= dt;
        else {
            mTimeLeft = 0;
            gameState = LEVEL_END;
            Mix_PauseMusic();
        }

        Item* collectedItem = Hook::getInstance().update(dt);
        if (collectedItem != nullptr) {
            if (!isMuted) Mix_PlayChannel(-1, mItemCollectSound, 0);
            score += collectedItem->getValue();

            items.erase(std::remove(items.begin(), items.end(), collectedItem), items.end());
            delete collectedItem;
        }

        if (Hook::getInstance().isExtending()) {
            SDL_Rect hookRect = Hook::getInstance().getTipRect();
            for (auto& item : items) {
                if (!item->isCaught() && SDL_HasIntersection(&hookRect, &item->getRect())) {
                    if (!isMuted) Mix_PlayChannel(-1, mItemHitSound, 0);
                    item->setCaught(true);
                    Hook::getInstance().attach(item);
                    break;
                }
            }
        }
        break;
    }
    case PAUSED:
        break;
    case LEVEL_END: case GAME_WON: case GAME_OVER:
        break;
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
    else if (gameState == GAME_WON) {
        TextureManager::Draw("winner", 0, 0, SCREEN_W, SCREEN_H);
    }
    else if (gameState == GAME_OVER) {
        TextureManager::Draw("game_over", 0, 0, SCREEN_W, SCREEN_H);
    }
    else if (gameState == LEVEL_START) {
        TextureManager::Draw("level_board", 0, 0, SCREEN_W, SCREEN_H);

        std::string levelText = "Man " + std::to_string(mCurrentLevel);
        std::string goalText = "Muc tieu: $" + std::to_string(mLevelGoal);
        SDL_Color black = { 0, 0, 0, 255 };

        SDL_Surface* levelSurface = TTF_RenderText_Blended(font, levelText.c_str(), black);
        SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
        SDL_Rect levelRect = { SCREEN_W / 2 - levelSurface->w / 2, SCREEN_H / 2 - levelSurface->h, levelSurface->w, levelSurface->h };
        SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);

        SDL_Surface* goalSurface = TTF_RenderText_Blended(font, goalText.c_str(), black);
        SDL_Texture* goalTexture = SDL_CreateTextureFromSurface(renderer, goalSurface);
        SDL_Rect goalRect = { SCREEN_W / 2 - goalSurface->w / 2, SCREEN_H / 2 + 20, goalSurface->w, goalSurface->h };
        SDL_RenderCopy(renderer, goalTexture, NULL, &goalRect);

        SDL_FreeSurface(levelSurface); SDL_DestroyTexture(levelTexture);
        SDL_FreeSurface(goalSurface); SDL_DestroyTexture(goalTexture);
    }
    else if (gameState == LEVEL_END) {
        TextureManager::Draw("level_board", 0, 0, SCREEN_W, SCREEN_H);

        if (score >= mLevelGoal) {
            SDL_RenderCopy(renderer, mLevelPassTexture, NULL, &mLevelPassRect);
        }
        else {
            SDL_RenderCopy(renderer, mLevelFailTexture, NULL, &mLevelFailRect);
        }
    }
    else {
        TextureManager::Draw("background", 0, 0, SCREEN_W, SCREEN_H);
        TextureManager::Draw("background2", 0, 5, SCREEN_W, 90);
        float pW = 150, pH = (int)(pW * (133.0f / 212.0f));
        TextureManager::Draw("player", (int)(SCREEN_W / 2.0f - pW / 2.0f), 5, pW, pH);

        for (auto& it : items) {
            it->render();
        }
        Hook::getInstance().render();

        SDL_Color green = { 0, 255, 0, 255 }, red = { 255, 0, 0, 255 }, white = { 255, 255, 255, 255 },
            yellow = { 255, 255, 0, 255 };

        std::string scoreText = "Tien: $" + std::to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Blended(mHudFont, scoreText.c_str(), green);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = { 80, 25, scoreSurface->w, scoreSurface->h };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurface); SDL_DestroyTexture(scoreTexture);

        std::string goalText = "Muc tieu: $" + std::to_string(mLevelGoal);
        SDL_Surface* goalSurface = TTF_RenderText_Blended(mHudFont, goalText.c_str(), yellow);
        SDL_Texture* goalTexture = SDL_CreateTextureFromSurface(renderer, goalSurface);
        SDL_Rect goalRect = { 80, scoreRect.y + 25, goalSurface->w, goalSurface->h };
        SDL_RenderCopy(renderer, goalTexture, NULL, &goalRect);
        SDL_FreeSurface(goalSurface); SDL_DestroyTexture(goalTexture);

        std::string timeText = "TG: " + std::to_string(static_cast<int>(mTimeLeft));
        SDL_Surface* timeSurface = TTF_RenderText_Blended(mHudFont, timeText.c_str(), red);
        SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_Rect timeRect = { 700, 25, timeSurface->w, timeSurface->h };
        SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);
        SDL_FreeSurface(timeSurface); SDL_DestroyTexture(timeTexture);

        std::string levelText = "Cap: " + std::to_string(mCurrentLevel);
        SDL_Surface* levelSurface = TTF_RenderText_Blended(mHudFont, levelText.c_str(), white);
        SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
        SDL_Rect levelRect = { 700, timeRect.y + 25, levelSurface->w, levelSurface->h };
        SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
        SDL_FreeSurface(levelSurface); SDL_DestroyTexture(levelTexture);

        TextureManager::Draw("pause_button", pauseButtonRect.x, pauseButtonRect.y, pauseButtonRect.w, pauseButtonRect.h);

        if (gameState == PAUSED) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(renderer, NULL);

            TextureManager::Draw("level_board", pauseMenuRect.x, pauseMenuRect.y, pauseMenuRect.w, pauseMenuRect.h);
            TextureManager::Draw("resume_button", resumeButtonRect.x, resumeButtonRect.y, resumeButtonRect.w, resumeButtonRect.h);
            TextureManager::Draw("main_menu_button", mainMenuButtonRect.x, mainMenuButtonRect.y, mainMenuButtonRect.w, mainMenuButtonRect.h);
        }
    }

    if (gameState != GAME_WON && gameState != GAME_OVER) {
        if (isMuted) TextureManager::Draw("sound_off", muteButtonRect.x, muteButtonRect.y, muteButtonRect.w, muteButtonRect.h);
        else TextureManager::Draw("sound_on", muteButtonRect.x, muteButtonRect.y, muteButtonRect.w, muteButtonRect.h);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TextureManager::Clear();

    if (startTextTexture) SDL_DestroyTexture(startTextTexture);
    if (mLevelPassTexture) SDL_DestroyTexture(mLevelPassTexture);
    if (mLevelFailTexture) SDL_DestroyTexture(mLevelFailTexture);
    if (font) TTF_CloseFont(font);
    if (mHudFont) TTF_CloseFont(mHudFont);

    if (gStartMusic) Mix_FreeMusic(gStartMusic);
    if (gGameplayMusic) Mix_FreeMusic(gGameplayMusic);
    if (mStartClickSound) Mix_FreeChunk(mStartClickSound);
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