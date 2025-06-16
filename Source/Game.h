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
    const int DEAD_ZONE = 8000;
    const float mTransitionTime = 0.2f;

    // Estados de movimento do analógico vertical
    enum class StickState {
        Neutral,
        Up,
        Down
    };

    enum class GameScene
    {
        MainMenu,
        Level1,
        Level2,
        Level3,
        Level4
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
    void UpdateActors(float deltaTime);
    void AddActor(class Actor *actor);
    void RemoveActor(class Actor *actor);

    // Draw functions
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }
    float GetLogicalWindowWidth() const { return mLogicalWindowWidth; }
    float GetLogicalWindowHeight() const { return mLogicalWindowHeight; }

    float GetScale() const { return mScale; }

    // Game-specific
    void AddGround(class Ground *g);
    void RemoveGround(class Ground *g);
    std::vector<class Ground *> &GetGrounds() { return mGrounds; }
    Ground* GetGroundById(int id);

    Player *GetPlayer() const { return mPlayer; }

    void UpdateCamera(float deltaTime);
    class Camera *GetCamera() const { return mCamera; }

    void AddFireBall(class FireBall *f);
    void RemoveFireball(class FireBall *f);
    std::vector<class FireBall *> &GetFireBalls() { return mFireBalls; }

    void AddParticle(class Particle *p);
    void RemoveParticle(class Particle *p);
    std::vector<class Particle *> &GetParticles() { return mParticles; }

    void AddProjectile(class Projectile *p);
    void RemoveProjectile(class Projectile *p);
    std::vector<class Projectile *> &GetProjectiles() { return mProjectiles; }

    void AddEnemy(class Enemy *e);
    void RemoveEnemy(class Enemy *e);
    std::vector<class Enemy *> &GetEnemies() { return mEnemies; }
    Enemy* GetEnemyById(int id);

    void SetResetLevel() { mResetLevel = true; }

    int **GetLevelData() const { return mLevelData; }
    int **GetLevelDataDynamicGrounds() const { return mLevelDataDynamicGrounds; }
    SDL_Texture* GetTileSheet() const { return mTileSheet; }
    std::unordered_map<int, SDL_Rect> GetTileSheetData() { return mSTileSheetData; }

    int GetTileSize() const { return mTileSize; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture *LoadTexture(const std::string &texturePath);

    int GetFPS() const { return mFPS; }

    void ActiveHitstop()
    {
        mHitstopActive = true;
        mHitstopTimer = 0;
    }

    // Audio functions
    class AudioSystem* GetAudio() const { return mAudio; }
    void SetMusicHandle(SoundHandle music) { mMusicHandle = music; }
    SoundHandle GetMusicHandle() const { return mMusicHandle; }
    void SetBossMusicHandle(SoundHandle music) { mBossMusic = music; }
    SoundHandle GetBossMusicHandle() const { return mBossMusic; }
    void StartBossMusic(SoundHandle music);
    void StopBossMusic();

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Scene management
    void SetGameScene(GameScene scene, float transitionTime = .0f);
    void ResetGameScene(float transitionTime = .0f);
    void UnloadScene();

    void TogglePause();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    SDL_Renderer* GetRenderer() const { return mRenderer; }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // Load Level
    void LoadObjects(const std::string &fileName);
    void LoadLevel(const std::string &fileName);
    void LoadMainMenu();
    UIScreen* LoadPauseMenu();

    void ChangeResolution(float oldScale);

    // All the actors in the game
    std::vector<class Actor *> mActors;
    std::vector<class Actor *> mPendingActors;

    // All the draw components
    std::vector<class DrawComponent *> mDrawables;

    // SDL stuff
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    AudioSystem* mAudio;

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
    bool mResetLevel;

    // Camera
    class Camera *mCamera;

    // Game-specific
    class Player *mPlayer;
    std::vector<class Ground *> mGrounds;
    std::vector<class FireBall *> mFireBalls;
    std::vector<class Particle *> mParticles;
    std::vector<class Projectile *> mProjectiles;
    std::vector<class Enemy *> mEnemies;
    SDL_GameController *mController;
    class HUD *mHUD;

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

    StickState mLeftStickYState;

    SoundHandle mMusicHandle;
    SoundHandle mBossMusic;

    // Scene management
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;
    float mFadeDuration;
    Uint8 mFadeAlpha;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;
    UIScreen* mPauseMenu;

    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;
    GameScene mContinueScene;


    void DrawParallaxBackground();
    void DrawParallaxLayer(SDL_Texture *texture, float parallaxFactor, int y, int h);

    SDL_Texture *mBackGroundTexture;

    SDL_Texture *mSky;
    SDL_Texture *mMountains;
    SDL_Texture *mTreesBack;
    SDL_Texture *mTreesFront;
};
