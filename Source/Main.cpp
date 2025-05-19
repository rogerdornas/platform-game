// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define SDL_MAIN_HANDLED

#include "Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
// const int SCREEN_WIDTH = 1280;
// const int SCREEN_HEIGHT = 720;
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 360;
// const int SCREEN_WIDTH = 1280;
// const int SCREEN_HEIGHT = 360;
const int FPS = 120;

int main(int argc, char **argv)
{
    // float scale;
    // float ratio = 1080.0 / 720.0;
    // int numTiles = 32 / ratio;
    // scale = numTiles / 32.0;
    // SDL_Log("ratio = %f", ratio);
    // SDL_Log("numTiles = %d", numTiles);
    // SDL_Log("scale = %f", scale);


    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT, FPS);
    bool success = game.Initialize();
    if (success)
    {
        game.RunLoop();
    }
    game.Shutdown();
    return 0;
}
