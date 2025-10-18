#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Gold Miner - SDL2", 800, 600)) {
        std::cerr << "Game init failed\n"; return -1;
    }

    while (game.running()) {
        game.handleEvents();
        game.update();
        game.render();
        SDL_Delay(16); 
    }

    game.clean();
    return 0;
}
