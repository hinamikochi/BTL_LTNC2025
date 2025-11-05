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

// Enum quản lý trạng thái game
enum GameState {
    START_SCREEN,
    LEVEL_START,
    PLAYING,
    PAUSED, 
    LEVEL_END,
    GAME_WON,
    GAME_OVER
};

// Struct định nghĩa thông tin mỗi màn chơi
struct LevelData {
    int goal;
    int time;
    int minGoldSmall, maxGoldSmall;
    int minGoldNormal, maxGoldNormal;
    int minGoldLarge, maxGoldLarge;
    int minStoneSmall, maxStoneSmall;
    int minStoneNormal, maxStoneNormal;
    int minStoneLarge, maxStoneLarge;
    int minDiamond, maxDiamond;
    int minGift, maxGift;
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
    GameState gameState;
    Uint32 lastTick;

    // Các biến cho màn hình khởi động
    TTF_Font* font;
    SDL_Texture* startTextTexture;
    SDL_Rect startTextRect;
    bool isStartTextVisible;
    float blinkTimer;

    // Âm thanh
    Mix_Music* gStartMusic;
    Mix_Music* gGameplayMusic;
    Mix_Chunk* mClawFireSound;
    Mix_Chunk* mItemHitSound;
    Mix_Chunk* mItemCollectSound;
    Mix_Chunk* mStartClickSound;
    Mix_Chunk* mGameWonSound;
    Mix_Chunk* mGameOverSound;

    // Nút mute
    bool isMuted;
    SDL_Rect muteButtonRect;

    // HUD và gameplay
    TTF_Font* mHudFont;
    int mLevelGoal;
    float mTimeLeft;
    int score;
    std::vector<Item*> items;

    // Hệ thống màn chơi
    int mCurrentLevel;
    std::vector<LevelData> mLevelConfigs;
    float mStateTimer;
    SDL_Texture* mLevelPassTexture;
    SDL_Rect mLevelPassRect;
    SDL_Texture* mLevelFailTexture;
    SDL_Rect mLevelFailRect;

    //  BIẾN PAUSE MENU 
    SDL_Rect pauseButtonRect; 
    SDL_Rect pauseMenuRect;   
    SDL_Rect resumeButtonRect; 
    SDL_Rect mainMenuButtonRect; 


    // Hàm helper
    void generateItemsForLevel(int level);
    void resetForNewLevel(int level);
};