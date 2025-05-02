// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>

#include "Camera.h"
#include "Actors/Ground.h"
#include "Actors/Player.h"
#include "Actors/FireBall.h"
#include "Actors/Enemy.h"
#include "Actors/EnemySimple.h"
#include "Actors/FlyingEnemySimple.h"

class Game
{
public:
    Game(int windowWidth, int windowHeight, int FPS);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Game-specific
    void AddGround(class Ground* g);
    void RemoveGround(class Ground* g);
    std::vector<class Ground*>& GetGrounds() { return mGrounds; }

    Player* GetPlayer() { return mPlayer; }

    void UpdateCamera(float deltatime);
    class Camera* GetCamera() { return mCamera; }

    void AddFireBall(class FireBall* f);
    void RemoveFireball(class FireBall* f);
    std::vector<class FireBall*>& GetFireBalls() { return mFireBalls; }

    void AddEnemy(class Enemy* e);
    void RemoveEnemy(class Enemy* e);
    std::vector<class Enemy*>& GetEnemys() { return mEnemys; }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mUpdatingActors;

    int mFPS;

    // Camera
    class Camera* mCamera;

    // Game-specific
    class Player* mPlayer;
    std::vector<class Ground*> mGrounds;
    std::vector<class FireBall*> mFireBalls;
    std::vector<class Enemy*> mEnemys;
    SDL_GameController* mController;
};
