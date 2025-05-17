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
#include "Math.h"
#include <string>

class Game
{
public:
    // int LEVEL_WIDTH = 0;
    // int LEVEL_HEIGHT = 0;
    // int TILE_SIZE = 0;
    // int SPAWN_DISTANCE = 700;

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

    float GetScale() { return mScale; }

    // Game-specific
    void AddGround(class Ground* g);
    void RemoveGround(class Ground* g);
    std::vector<class Ground*>& GetGrounds() { return mGrounds; }

    Player* GetPlayer() { return mPlayer; }

    void UpdateCamera(float deltaTime);
    class Camera* GetCamera() { return mCamera; }

    void AddFireBall(class FireBall* f);
    void RemoveFireball(class FireBall* f);
    std::vector<class FireBall*>& GetFireBalls() { return mFireBalls; }

    void AddEnemy(class Enemy* e);
    void RemoveEnemy(class Enemy* e);
    std::vector<class Enemy*>& GetEnemys() { return mEnemys; }

    int **GetLevelData() const { return mLevelData; }
    int GetTileSize() { return mTileSize; }

    bool mResetLevel;

    SDL_Texture* LoadTexture(const std::string& texturePath);

    int GetFPS() { return mFPS; }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // Load the level from a CSV file as a 2D array
    void LoadMapMetadata(const std::string &fileName);
    int **LoadLevel(const std::string& fileName, int width, int height);
    void LoadObjects(const std::string& fileName);

    void ResetLevel();

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
    const float mOriginalWindowWidth = 1920;
    const float mOriginalWindowHeight = 1080;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mUpdatingActors;

    int mFPS;

    bool mIsPaused;

    // Camera
    class Camera* mCamera;

    // Game-specific
    class Player* mPlayer;
    std::vector<class Ground*> mGrounds;
    std::vector<class FireBall*> mFireBalls;
    std::vector<class Enemy*> mEnemys;
    SDL_GameController* mController;

    // Level data
    int **mLevelData;
    int mLevelWidth;
    int mLevelHeight;
    int mTileSize;

    float mScale;

    void DrawParallaxBackground();
    void DrawParallaxLayer(SDL_Texture* texture, float parallaxFactor, int y, int h);

    SDL_Texture* mBackGroundTexture;

    SDL_Texture* mSky;
    SDL_Texture* mMountains;
    SDL_Texture* mTreesBack;
    SDL_Texture* mTreesFront;
};
