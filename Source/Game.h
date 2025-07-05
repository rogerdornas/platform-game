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
#include "Cutscene.h"
#include "Store.h"

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

    enum class GameScene {
        MainMenu,
        LevelTeste,
        Prologue,
        Level1,
        Level2,
        Level3,
        Level4,
        Level5
    };

    enum class SceneManagerState {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving,
        Cutscene,
        Menu
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

    class Player* GetPlayer() const { return mPlayer; }

    void UpdateCamera(float deltaTime);
    class Camera* GetCamera() const { return mCamera; }

    void AddFireBall(class FireBall *f);
    void RemoveFireball(class FireBall *f);
    std::vector<class FireBall*> &GetFireBalls() { return mFireBalls; }

    void AddParticle(class Particle *p);
    void RemoveParticle(class Particle *p);
    std::vector<class Particle*> &GetParticles() { return mParticles; }

    void AddProjectile(class Projectile *p);
    void RemoveProjectile(class Projectile *p);
    std::vector<class Projectile *> &GetProjectiles() { return mProjectiles; }

    void AddMoney(class Money *m);
    void RemoveMoney(class Money *m);
    std::vector<class Money*> &GetMoneys() { return mMoneys; }

    void AddEnemy(class Enemy *e);
    void RemoveEnemy(class Enemy *e);
    std::vector<class Enemy*> &GetEnemies() { return mEnemies; }
    Enemy* GetEnemyById(int id);

    void SetResetLevel() { mResetLevel = true; }

    int **GetLevelData() const { return mLevelData; }
    int **GetLevelDataDynamicGrounds() const { return mLevelDataDynamicGrounds; }
    SDL_Texture* GetTileSheet() const { return mTileSheet; }
    std::unordered_map<int, SDL_Rect> GetTileSheetData() { return mTileSheetData; }

    int GetTileSize() const { return mTileSize; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture *LoadTexture(const std::string &texturePath);

    int GetFPS() const { return mFPS; }

    void ActiveHitStop()
    {
        mHitstopDelayActive = true;
        mHitstopDelayTimer = 0;
        mHitstopTimer = 0;
    }

    void SetIsSlowMotion(bool slowMotion) { mIsSlowMotion = slowMotion; }

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
    class HUD* GetHUD() const { return mHUD; }

    // Scene management
    void SetGameScene(GameScene scene, float transitionTime = .0f);
    void ResetGameScene(float transitionTime = .0f);
    void UnloadScene();
    GameScene GetGameScene() const { return mGameScene; }

    void TogglePause();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    bool GetIsPlayingOnKeyboard() const { return mIsPlayingOnKeyboard; }

    SDL_Renderer* GetRenderer() const { return mRenderer; }

    Store* GetStore() const { return mStore; }

    void PlayFinalGoodCutscene();
    void PlayFinalEvilCutscene();
    void SetIsPlayingFinalCutscene() { mIsPlayingFinalCutscene = true; }

    std::vector<std::string> mGoodCutscenes;
    std::vector<GameScene> mGoodCutsceneScenes;
    std::vector<std::string> mEvilCutscenes;
    int mCutsceneIndex;

    void SetCheckPointPosition(Vector2 pos) { mCheckpointPosition = pos; }
    Vector2 GetCheckPointPosition() { return mCheckpointPosition; }
    void SetCheckPointMoney(int money) { mCheckPointMoney = money; }
    int GetCheckPointMoney() const { return mCheckPointMoney; }
    void SetGoingToNextLevel() { mGoingToNextLevel = true; }
    void SetCurrentCutscene(Cutscene* cutscene) { mCurrentCutscene = cutscene; }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // Load Level
    void LoadObjects(const std::string &fileName);
    void LoadLevel(const std::string &fileName);
    void LoadMainMenu();
    UIScreen* LoadPauseMenu();
    void LoadLevelSelectMenu();
    void LoadOptionsMenu();
    void LoadControlMenu();
    void LoadKeyBoardMenu();

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
    std::vector<class Ground*> mGrounds;
    std::vector<class FireBall*> mFireBalls;
    std::vector<class Particle*> mParticles;
    std::vector<class Projectile*> mProjectiles;
    std::vector<class Money*> mMoneys;
    std::vector<class Enemy *> mEnemies;
    SDL_GameController *mController;
    class HUD *mHUD;
    std::vector<class Checkpoint*> mCheckPoints;

    int mPlayerDeathCounter;

    // Player State
    Vector2 mCheckpointPosition;
    int mCheckPointMoney;
    bool mGoingToNextLevel;

    // Level data
    int **mLevelData;
    int **mLevelDataDynamicGrounds;
    SDL_Texture* mTileSheet;
    std::unordered_map<int, SDL_Rect> mTileSheetData;
    int mLevelWidth;
    int mLevelHeight;
    int mTileSize;
    const int mOriginalTileSize = 32;

    float mScale;

    bool mHitstopActive;
    float mHitstopDuration;
    float mHitstopTimer;
    bool mHitstopDelayActive;
    float mHitstopDelayDuration;
    float mHitstopDelayTimer;

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
    UIScreen* mOptionsMenu;
    UIScreen* mLevelSelectMenu;
    UIScreen* mControlMenu;
    UIScreen* mKeyboardMenu;

    std::vector<Vector2> mResolutions = {
        Vector2(640, 360),
        Vector2(1280, 720),
        Vector2(1920, 1080),
    };
    int mCurrentResolutionIndex = 1;

    GamePlayState mGamePlayState;

    // If is playing on controller or keyboard
    bool mIsPlayingOnKeyboard;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;
    GameScene mContinueScene;

    Cutscene* mCurrentCutscene;
    bool mIsPlayingFinalCutscene;

    Store* mStore;

    // Background images
    void DrawParallaxBackground(SDL_Texture* background);
    void DrawParallaxLayer(SDL_Texture *texture, float parallaxFactor, int y, int h);
    void DrawParallaxLayers(std::vector<SDL_Texture*> backgroundLayers);

    bool mUseParallaxBackground;
    SDL_Texture* mBackGroundTextureMainMenu;
    SDL_Texture* mBackGroundTextureLevel1;
    std::vector<SDL_Texture*> mBackgroundLayersLevel2;
    std::vector<SDL_Texture*> mBackgroundLayersLevel3;
    std::vector<SDL_Texture*> mBackgroundLayersLevel4;
    SDL_Texture* mBackGroundTextureLevel3;
    SDL_Texture* mBackGroundTextureLevel4;
    SDL_Texture* mBackGroundTexture;

    // SDL_Texture *mSky;
    // SDL_Texture *mMountains;
    // SDL_Texture *mTreesBack;
    // SDL_Texture *mTreesFront;
};
