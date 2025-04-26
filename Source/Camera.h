//
// Created by roger on 23/04/2025.
//

#pragma once
#include "Math.h"
#include "Game.h"

class Camera {
public:
    Camera(class Game* game);

    Vector2 GetPosCamera() { return mPos; }
    void Update();

private:
    Vector2 mPos;
    class Game* mGame;
};


