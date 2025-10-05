#pragma once
#include <SDL.h>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "Item.h"
#include "Gold.h"
#include "Stone.h"
#include "Diamond.h" 
#include "Gift.h" 

// Enum để quản lý trạng thái của game
enum GameState {
    START_SCREEN,
    PLAYING
};

class Game {
public:
    Game();
    ~Game();
    bool init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() const { return isRunning; }

    static SDL_Renderer* renderer;
    static int SCREEN_W;
    static int SCREEN_H;

private:
    SDL_Window* window;
    bool isRunning;

    // Các biến cho màn hình khởi động
    GameState gameState;
    TTF_Font* font;
    SDL_Texture* startTextTexture;
    SDL_Rect startTextRect;

    // Các biến cho hiệu ứng chớp nháy
    bool isStartTextVisible;
    float blinkTimer;

    // Biến cho âm thanh
    Mix_Music* gStartMusic;
    Mix_Chunk* mStartSound;

    Mix_Music* gGameplayMusic;
    Mix_Chunk* mClawFireSound;
    Mix_Chunk* mItemHitSound;
    Mix_Chunk* mItemCollectSound;
    Mix_Chunk* mStartClickSound;

    // Biến cho nút mute
    bool isMuted;
    SDL_Rect muteButtonRect;

    // 
    TTF_Font* mHudFont;
    int mLevelGoal;
    float mTimeLeft;

    // Các biến cho gameplay
    std::vector<Item*> items;
    int score;
    Uint32 lastTick;

    void loadItems();
};