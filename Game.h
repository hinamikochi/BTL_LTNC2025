#pragma once
#include <SDL.h>
#include <vector>
#include "Item.h"
#include "Gold.h"
#include "Stone.h"

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

   
    std::vector<Item*> items;

    int score;
    Uint32 lastTick;

    void loadItems();  // tạo vàng và đá ban đầu
};
