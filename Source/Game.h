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
#include <unordered_map>
#include "AudioSystem.h"

class Game
{
public:
    static const int TRANSITION_TIME = 1;

    enum class GameScene
    {
        MainMenu,
        Level1,
        Level2,
        Level3
    };

    enum class SceneManagerState
    {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState
    {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving
    };

    Game(int windowWidth, int windowHeight, int FPS);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor *actor);
    void RemoveActor(class Actor *actor);

    // Draw functions
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }
    float GetLogicalWindowWidth() { return mLogicalWindowWidth; }
    float GetLogicalWindowHeight() { return mLogicalWindowHeight; }

    float GetScale() { return mScale; }

    // Game-specific
    void AddGround(class Ground *g);
    void RemoveGround(class Ground *g);
    std::vector<class Ground *> &GetGrounds() { return mGrounds; }
    Ground* GetGroundById(int id);

    Player *GetPlayer() { return mPlayer; }

    void UpdateCamera(float deltaTime);
    class Camera *GetCamera() { return mCamera; }

    void AddFireBall(class FireBall *f);
    void RemoveFireball(class FireBall *f);
    std::vector<class FireBall *> &GetFireBalls() { return mFireBalls; }

    void AddParticle(class Particle *p);
    void RemoveParticle(class Particle *p);
    std::vector<class Particle *> &GetParticles() { return mParticles; }

    void AddEnemy(class Enemy *e);
    void RemoveEnemy(class Enemy *e);
    std::vector<class Enemy *> &GetEnemies() { return mEnemies; }

    int **GetLevelData() const { return mLevelData; }
    int **GetLevelDataDynamicGrounds() const { return mLevelDataDynamicGrounds; }
    SDL_Texture* GetTileSheet() const { return mTileSheet; }
    std::unordered_map<int, SDL_Rect> GetTileSheetData() { return mSTileSheetData; }

    int GetTileSize() { return mTileSize; }

    bool mResetLevel;

    SDL_Texture *LoadTexture(const std::string &texturePath);

    int GetFPS() { return mFPS; }

    void ActiveHitstop()
    {
        mHitstopActive = true;
        mHitstopTimer = 0;
    }

    // Audio functions
    class AudioSystem* GetAudio() { return mAudio; }
    void SetMusicHandle(SoundHandle music) { mMusicHandle = music; }
    SoundHandle GetMusicHandle() { return mMusicHandle; }
    void StarBossMusic(SoundHandle music);
    void StopBossMusic();

    // Scene management
    void SetGameScene(GameScene scene, float transitionTime = .0f);
    void ResetGameScene(float transitionTime = .0f);
    void UnloadScene();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // Load Level
    void LoadObjects(const std::string &fileName);
    void LoadLevel(const std::string &fileName);

    void ResetLevel();
    void ChangeResolution(float oldScale);

    // All the actors in the game
    std::vector<class Actor *> mActors;
    std::vector<class Actor *> mPendingActors;

    // All the draw components
    std::vector<class DrawComponent *> mDrawables;

    // SDL stuff
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;
    const float mOriginalWindowWidth = 1920;
    const float mOriginalWindowHeight = 1080;
    float mLogicalWindowWidth;
    float mLogicalWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mUpdatingActors;

    int mFPS;

    bool mIsPaused;

    // Camera
    class Camera *mCamera;

    // Game-specific
    class Player *mPlayer;
    std::vector<class Ground *> mGrounds;
    std::vector<class FireBall *> mFireBalls;
    std::vector<class Particle *> mParticles;
    std::vector<class Enemy *> mEnemies;
    SDL_GameController *mController;

    // Level data
    int **mLevelData;
    int **mLevelDataDynamicGrounds;
    SDL_Texture* mTileSheet;
    std::unordered_map<int, SDL_Rect> mSTileSheetData;
    int mLevelWidth;
    int mLevelHeight;
    int mTileSize;
    const int mOriginalTileSize = 32;

    float mScale;

    bool mHitstopActive;
    float mHitstopDuration;
    float mHitstopTimer;

    bool mIsSlowMotion;
    bool mIsAccelerated;

    AudioSystem* mAudio;
    SoundHandle mMusicHandle;
    SoundHandle mBossMusic;

    // Scene management
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;

    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;


    void DrawParallaxBackground();
    void DrawParallaxLayer(SDL_Texture *texture, float parallaxFactor, int y, int h);

    SDL_Texture *mBackGroundTexture;

    SDL_Texture *mSky;
    SDL_Texture *mMountains;
    SDL_Texture *mTreesBack;
    SDL_Texture *mTreesFront;
};
