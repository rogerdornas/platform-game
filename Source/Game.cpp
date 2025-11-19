#include <algorithm>
#include <vector>
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/ParticleSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "CSV.h"
#include "Json.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "HUD.h"
#include "SaveData.h"
#include "SaveManager.h"
#include "Actors/BushMonster.h"
#include "Actors/Checkpoint.h"
#include "Actors/CloneEnemy.h"
#include "Actors/Decorations.h"
#include "Actors/DragonFly.h"
#include "UIElements/UIScreen.h"
#include "Actors/DynamicGround.h"
#include "Actors/Fox.h"
#include "Actors/Frog.h"
#include "Actors/Lever.h"
#include "Actors/Trigger.h"
#include "Actors/Fairy.h"
#include "Actors/EnemySimple.h"
#include "Actors/FlyingEnemySimple.h"
#include "Actors/FlyingGolem.h"
#include "Actors/FlyingShooterEnemy.h"
#include "Actors/FlyingSpawnerEnemy.h"
#include "Actors/Golem.h"
#include "Actors/HookEnemy.h"
#include "Actors/HookPoint.h"
#include "Actors/Lava.h"
#include "Actors/LittleBat.h"
#include "Actors/Mantis.h"
#include "Actors/MirrorBoss.h"
#include "Actors/Money.h"
#include "Actors/Moth.h"
#include "Actors/Mushroom.h"
#include "Actors/Projectile.h"
#include "Actors/Snake.h"
#include "Components/AABBComponent.h"
#include "Components/DashComponent.h"
#include "Components/Drawing/AnimatorComponent.h"
#include "Components/Drawing/RectComponent.h"

std::vector<int> ParseIntList(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            result.push_back(std::stoi(item));
        }
    }

    return result;
}

Game::Game(int windowWidth, int windowHeight, int FPS)
    :mBackToCheckpoint(false)
    ,mWindow(nullptr)
    ,mRenderer(nullptr)
    ,mWindowWidth(windowWidth)
    ,mWindowHeight(windowHeight)
    ,mLogicalWindowWidth(windowWidth)
    ,mLogicalWindowHeight(windowHeight)
    ,mTicksCount(0)
    ,mIsRunning(true)
    ,mUpdatingActors(false)
    ,mFPS(FPS)
    ,mIsPaused(false)
    ,mIsCrossFading(false)
    ,mCrossFadeDuration(0.0f)
    ,mCrossFadeTimer(0.0f)
    ,mCamera(nullptr)
    ,mPlayer(nullptr)
    ,mStore(nullptr)
    ,mScale(1.0f)
    ,mLevelData(nullptr)
    ,mLevelDataDynamicGrounds(nullptr)
    ,mMap(nullptr)
    ,mShowMap(false)
    ,mTileSheet(nullptr)
    ,mController(nullptr)
    ,mHitstopActive(false)
    ,mHitstopDuration(0.15f)
    ,mHitstopTimer(0.0f)
    ,mHitstopDelayActive(false)
    ,mHitstopDelayDuration(0.01f)
    ,mHitstopDelayTimer(0.0f)
    ,mIsSlowMotion(false)
    ,mIsAccelerated(false)
    ,mSaveSlot(0)
    ,mSaveData(nullptr)
    ,mSaveManager(nullptr)
    ,mPlayerDeathCounter(0)
    ,mCheckpointPosition(Vector2::Zero)
    ,mCheckpointGameScene(GameScene::Prologue)
    ,mLavaRespawnPosition(Vector2::Zero)
    ,mHitByLava(false)
    ,mPlayerStartPositionId(0)
    ,mCheckPointMoney(0)
    ,mGoingToNextLevel(false)
    ,mIsPlayingOnKeyboard(true)
    ,mLeftStickStateY(StickState::Neutral)
    ,mLeftStickStateX(StickState::Neutral)
    ,mWaveManager(nullptr)
    ,mNewButtonText(nullptr)
    ,mWaitingForKey(false)
    ,mIsPlayingFinalCutscene(false)
    ,mCurrentCutscene(nullptr)
    ,mBackGroundTexture(nullptr)
    ,mBackGroundTextureMainMenu(nullptr)
    ,mBackGroundTextureLevel1(nullptr)
    ,mBackGroundTextureLevel3(nullptr)
    ,mBackGroundTextureLevel4(nullptr)
    ,mUseParallaxBackground(false)
    ,mAudio(nullptr)
    ,mHUD(nullptr)
    ,mMainMenu(nullptr)
    ,mPauseMenu(nullptr)
    ,mOptionsMenu(nullptr)
    ,mLevelSelectMenu(nullptr)
    ,mControlMenu(nullptr)
    ,mKeyboardMenu(nullptr)
    ,mKeyboardMenu2(nullptr)
    ,mConfirmBackToMenu(nullptr)
    ,mConfirmQuitGameMenu(nullptr)
    ,mLoadGameMenu(nullptr)
    ,mSceneManagerState(SceneManagerState::None)
    ,mFadeDuration(0.4f)
    ,mSceneManagerTimer(0.0f)
    ,mFadeAlpha(0)
    ,mGameScene(GameScene::Room0)
    ,mNextScene(GameScene::Room0)
{
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Echoes of Elementum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               mWindowWidth, mWindowHeight,
                               // SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(mWindowWidth, mWindowHeight);

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Informe o renderer sobre a mudança
    if (mRenderer) // 'mRenderer' é seu objeto Renderer
    {
        mRenderer->OnWindowResize(mWindowWidth, mWindowHeight);
    }

    mLogicalWindowWidth = mRenderer->GetVirtualWidth();
    mLogicalWindowHeight = mRenderer->GetVirtualHeight();

    // if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
    //     mLogicalWindowWidth = static_cast<float>(mWindowWidth);
    //     mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
    //     SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
    //     float ratio = mOriginalWindowWidth / static_cast<float>(mLogicalWindowWidth);
    //     int tileSize = static_cast<int>(mOriginalTileSize / ratio);
    //     mScale = static_cast<float>(tileSize) / mOriginalTileSize;
    // }
    // else {
    //     mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
    //     mLogicalWindowHeight = static_cast<float>(mWindowHeight);
    //     SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
    //     float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
    //     int tileSize = static_cast<int>(mOriginalTileSize / ratio);
    //     mScale = static_cast<float>(tileSize) / mOriginalTileSize;
    // }


    // Esconde o cursor
    // SDL_ShowCursor(SDL_DISABLE);

    // Inicializa controle
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if (SDL_IsGameController(i))
        {
            mController = SDL_GameControllerOpen(i);
            if (mController)
                break;
        }
    }

    LoadBindingsFromFile("../Assets/InputBindings/InputBindings.json");

    Random::Init();

    // Load Audios
    mAudio = new AudioSystem(16);
    mAudio->CacheSound("Hornet.wav");
    mAudio->CacheSound("MantisLords.wav");
    mAudio->CacheSound("HollowKnight.wav");

    // Load Final cutscenes
    mGoodCutscenes = {"ShowLevel2", "ShowLevel3", "ShowLevel4"};
    mGoodCutsceneScenes = {GameScene::Level2, GameScene::Level3, GameScene::Level4};
    mEvilCutscenes = {};
    mCutsceneIndex = 0;

    // Load Background Images
    const std::string backgroundAssets = "../Assets/Sprites/Background/";

    // mBackGroundTextureMainMenu = LoadTexture(backgroundAssets + "Menu6.png");
    //
    // mBackGroundTextureLevel1 = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
    // mBackGroundTextureLevel3 = LoadTexture(backgroundAssets + "fundoCortadoEspichado.png");
    // mBackGroundTextureLevel4 = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art4.png");

    // mRenderer->GetTexture(backgroundAssets + "Level2/6.png");
    // mRenderer->GetTexture(backgroundAssets + "Level2/5.png");
    // mRenderer->GetTexture(backgroundAssets + "Level2/4.png");
    // mRenderer->GetTexture(backgroundAssets + "Level2/3.png");
    // mRenderer->GetTexture(backgroundAssets + "Level2/2.png");
    // mRenderer->GetTexture(backgroundAssets + "Level2/1.png");

    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/7.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/6.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/5.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/4.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/3.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/2.png"));
    // mBackgroundLayersLevel2.emplace_back(mRenderer->GetTexture(backgroundAssets + "Level2/1.png"));

    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/7.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/6.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/5.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/4.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/3.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/2.png"));
    // mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/1.png"));
    //
    // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/1.png"));
    // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/2.png"));
    // // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/3.png"));
    // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/4.png"));
    // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/5.png"));
    //
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/1.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/2.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/3.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/4.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/5.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/6.png"));
    // mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/7.png"));

    mTicksCount = SDL_GetTicks();

    SetGameScene(GameScene::MainMenu);

    mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

    mSaveData = new SaveData(this);
    mSaveManager = new SaveManager(this);

    // const std::string backgroundAssets = "../Assets/Sprites/Background/";
    // mBackGroundTexture = LoadTexture(backgroundAssets + "fundoCortadoEspichado.png");
    // mBackGroundTexture = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
    // mBackGroundTexture = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art4.png");
    // mSky = LoadTexture(backgroundAssets + "sky_cloud.png");
    // mMountains = LoadTexture(backgroundAssets + "mountain2.png");
    // mTreesBack = LoadTexture(backgroundAssets + "pine1.png");
    // mTreesFront = LoadTexture(backgroundAssets + "pine2.png");

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime) {
    // Verifica se o gerenciador de cenas está pronto para uma nova transição
    if (mSceneManagerState == SceneManagerState::None) {
        // Verifica se a cena é válida
        if (scene == GameScene::MainMenu ||
            scene == GameScene::LevelTeste ||
            scene == GameScene::Coliseu ||
            scene == GameScene::Prologue ||
            scene == GameScene::Level1 ||
            scene == GameScene::Level2 ||
            scene == GameScene::Level3 ||
            scene == GameScene::Level4 ||
            scene == GameScene::Level5 ||
            scene == GameScene::Room0 ||
            scene == GameScene::MirrorBoss) {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
        }
        else {
            SDL_Log("SetGameScene: Cena inválida passada como parâmetro.");
            return;
        }
    }
    else {
        SDL_Log("SetGameScene: Já há uma transição de cena em andamento.");
        return;
    }
}

void Game::ResetGameScene(float transitionTime)
{
    mIsAccelerated = false;
    mIsSlowMotion = false;
    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    mIsSlowMotion = false;
    mIsAccelerated = false;

    if (mGamePlayState != GamePlayState::Cutscene) {
        // mAudio->StopAllSounds();
        mAudio->StopSound(mMusicHandle);
        mAudio->StopSound(mBossMusic);
    }

    const std::string backgroundAssets = "../Assets/Sprites/Background/";
    const std::string levelsAssets = "../Assets/Levels/";

    if (mNextScene != GameScene::MainMenu) {
        // Pool de Fireballs
        for (int i = 0; i < 30; i++) {
            new FireBall(this);
        }

        // Pool de Partículas
        for (int i = 0; i < 200; i++) {
            new Particle(this);
        }

        // Pool de Projectiles
        for (int i = 0; i < 50; i++) {
            new Projectile(this, Projectile::ProjectileType::Acid);
        }
        for (int i = 0; i < 50; i++) {
            new Projectile(this, Projectile::ProjectileType::OrangeBall);
        }

        // Pool de Moneys
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Small);
        }
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Medium);
        }
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Large);
        }

        // não carrega companheiro na última fase
        if (mNextScene != GameScene::Level5) {
            // auto* fairy = new Fairy(this, 40, 40);
        }

        // Volta player
        if (mPlayer) {
            if (mPlayer->GetComponent<AnimatorComponent>()) {
                mPlayer->GetComponent<AnimatorComponent>()->SetVisible(true);
            }
            if (mPlayer->GetComponent<RectComponent>()) {
                mPlayer->GetComponent<RectComponent>()->SetVisible(true);
            }
        }

        // Guarda último level que o player estava
        mIsPaused = false;
    }
    else {
        // Se está no menu, pausa draw de player
        if (mPlayer) {
            if (mPlayer->GetComponent<AnimatorComponent>()) {
                mPlayer->GetComponent<AnimatorComponent>()->SetVisible(false);
            }
            if (mPlayer->GetComponent<RectComponent>()) {
                mPlayer->GetComponent<RectComponent>()->SetVisible(false);
            }
            // if (mPlayer->GetComponent<DrawRopeComponent>()) {
            //     mPlayer->GetComponent<DrawRopeComponent>()->SetVisible(false);
            // }
        }
        // Delete map
        // if (mMap) {
        //     delete mMap;
        //     mMap = nullptr;
        // }
    }

    // Reset gameplay state
    if (!mIsPlayingFinalCutscene) {
        mGamePlayState = GamePlayState::Playing;
    }

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu) {
        mUseParallaxBackground = false;
        mGamePlayState = GamePlayState::Menu;
        mCutsceneIndex = 0;
        mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Menu6.png");

        // Initialize main menu actors
        LoadMainMenu();

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("HollowKnight.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::LevelTeste) {
        mUseParallaxBackground = true;
        LoadLevel(levelsAssets + "Forest/Forest.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Coliseu) {
        mUseParallaxBackground = true;
        LoadLevel(levelsAssets + "Coliseu/Coliseu.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Prologue) {
        mUseParallaxBackground = false;
        // mUseParallaxBackground = true;
        mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
        LoadLevel(levelsAssets + "0-Prologue/Prologue.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Level1) {
        mUseParallaxBackground = true;
        LoadLevel(levelsAssets + "1-Musgo/Musgo.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Level2) {
        mUseParallaxBackground = true;
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/7.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/6.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/5.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/4.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/3.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/2.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level2/1.png"));

        LoadLevel(levelsAssets + "2-Run/Run.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Level3) {
        mUseParallaxBackground = true;

        LoadLevel(levelsAssets + "3-Swamp/Swamp.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Level4) {
        mUseParallaxBackground = true;
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/7.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/6.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/5.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/4.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/3.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/2.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/1.png"));

        LoadLevel(levelsAssets + "4-Pain/Pain.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    else if (mNextScene == GameScene::Level5) {
        mUseParallaxBackground = true;
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/7.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/6.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/5.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/4.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/3.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/2.png"));
        // mBackgroundLayers.emplace_back(LoadTexture(backgroundAssets + "Level4/1.png"));

        LoadLevel(levelsAssets + "5-FinalLevel/Level5.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }
    else if (mNextScene == GameScene::Room0) {
        mUseParallaxBackground = false;
        // mUseParallaxBackground = true;
        mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
        // mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Level2/4.png");

        LoadLevel(levelsAssets + "Room0/Room0.json");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }
    else if (mNextScene == GameScene::MirrorBoss) {
        LoadLevel(levelsAssets + "MirrorBoss/MirrorBoss.json");

        mUseParallaxBackground = false;
        mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");

        if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
            mMusicHandle = mAudio->PlaySound("Greenpath.wav", true);
        }
        mBossMusic.Reset();
    }

    // Verifica as 2 primeiras mortes do player para tocar cutscene
    // if (mPlayerDeathCounter < 3 && mPlayer) {
    //     if (mPlayerDeathCounter < mPlayer->GetDeathCounter() && mGamePlayState == GamePlayState::Playing) {
    //         mPlayerDeathCounter = mPlayer->GetDeathCounter();
    //         if (mPlayerDeathCounter == 1) {
    //             mCurrentCutscene = new Cutscene(this, "primeiraMortePlayer", "../Assets/Cutscenes/Cutscenes.json");
    //             mCurrentCutscene->Start();
    //             SetCurrentCutscene(mCurrentCutscene);
    //             SetGamePlayState(Game::GamePlayState::Cutscene);
    //         }
    //         if (mPlayerDeathCounter == 2) {
    //             mCurrentCutscene = new Cutscene(this, "segundaMortePlayer", "../Assets/Cutscenes/Cutscenes.json");
    //             mCurrentCutscene->Start();
    //             SetCurrentCutscene(mCurrentCutscene);
    //             SetGamePlayState(Game::GamePlayState::Cutscene);
    //         }
    //     }
    // }

    // Set new scene
    mGameScene = mNextScene;
}

void Game::LoadMainMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mMainMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(virtualWidth / 5, 0.046f * virtualHeight);
    mMainMenu->SetSize(Vector2(virtualWidth / 3, virtualHeight / 3));
    mMainMenu->SetPosition(Vector2(virtualWidth / 3, 2 * virtualHeight / 3));
    Vector2 buttonPos = Vector2((mMainMenu->GetSize().x - buttonSize.x) / 2, 0.0f);
    float distanceBetweenButtons = 0.064f * virtualHeight;

    std::string name = "INICIAR JOGO";
    int buttonPointSize = static_cast<int>(0.031f * virtualHeight);
    mMainMenu->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        LoadLoadGameMenu();
    });

    name = "OPÇÕES";
    mMainMenu->AddButton(name, buttonPos + Vector2(0, 2 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() { LoadOptionsMenu(); });

    name = "SAIR";
    mMainMenu->AddButton(name, buttonPos + Vector2(0, 3 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mMainMenu->SetIsVisible(false);
        LoadConfirmQuitGameMenu();
    });
}

void Game::LoadConfirmQuitGameMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mConfirmQuitGameMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(virtualWidth / 5, 0.046f * virtualHeight);
    mConfirmQuitGameMenu->SetSize(Vector2(virtualWidth / 3, virtualHeight / 3));
    mConfirmQuitGameMenu->SetPosition(Vector2(virtualWidth / 3, 2 * virtualHeight / 3));
    Vector2 buttonPos = Vector2((mConfirmQuitGameMenu->GetSize().x - buttonSize.x) / 2, mConfirmQuitGameMenu->GetSize().y * 0.30f);
    float distanceBetweenButtons = 0.064f * virtualHeight;

    UIText* text = mConfirmQuitGameMenu->AddText("SAIR DO JOGO?", Vector2::Zero, Vector2::Zero, 0.035f * mWindowHeight);
    text->SetPosition(Vector2(mConfirmQuitGameMenu->GetSize().x / 2, 0.0f));

    std::string name = "SIM";
    int buttonPointSize = static_cast<int>(0.031f * virtualHeight);
    mConfirmQuitGameMenu->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        Quit();
    });

    name = "NÃO";
    mConfirmQuitGameMenu->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mConfirmQuitGameMenu->Close();
        mMainMenu->SetIsVisible(true);
    });
}

void Game::LoadLoadGameMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mLoadGameMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mLoadGameMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mLoadGameMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));

    auto buttonSize = Vector2(mLoadGameMenu->GetSize().x * 0.65f, 0.14f * virtualHeight);
    auto buttonPointSize = static_cast<int>(0.033f * virtualHeight);
    auto buttonPos = Vector2(mLoadGameMenu->GetSize().x * 0.05f, mLoadGameMenu->GetSize().y * 0.2f);
    float distanceBetweenButtons = 0.16f * virtualHeight;

    mLoadGameMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mLoadGameMenu->GetSize() / 2, mLoadGameMenu->GetSize());

    UIText* text = mLoadGameMenu->AddText("SELECIONAR PERFIL", Vector2::Zero, Vector2::Zero, 0.046f * virtualHeight);
    text->SetPosition(Vector2(mLoadGameMenu->GetSize().x / 2, mLoadGameMenu->GetSize().y * 0.05f));

    std::string name = "   SLOT 1";
    mLoadGameMenu->AddButton(name, buttonPos,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

            mSaveSlot = 1;
            LoadGame();
        });

    name = "   SLOT 2";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

            mSaveSlot = 2;
            LoadGame();
        });

    name = "   SLOT 3";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 2 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

            mSaveSlot = 3;
            LoadGame();
        });

    name = "   SLOT 4";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 3 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

            mSaveSlot = 4;
            LoadGame();
        });

    buttonSize = Vector2(mLoadGameMenu->GetSize().x * 0.20f, 0.046f * virtualHeight);
    buttonPos = Vector2(mLoadGameMenu->GetSize().x * 0.75f, mLoadGameMenu->GetSize().y * 0.25f);

    name = "DELETAR SAVE";
    mLoadGameMenu->AddButton(name, buttonPos,
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]()
        {
            mSaveManager->DeleteSave(1);
        });

    name = "DELETAR SAVE";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]()
        {
            mSaveManager->DeleteSave(2);
        });

    name = "DELETAR SAVE";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 2 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]()
        {
            mSaveManager->DeleteSave(3);
        });

    name = "DELETAR SAVE";
    mLoadGameMenu->AddButton(name, buttonPos + Vector2(0, 3 * distanceBetweenButtons),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]()
        {
            mSaveManager->DeleteSave(4);
        });

    buttonSize = Vector2(mLoadGameMenu->GetSize().x * 0.2f, 0.046f * virtualHeight);
    buttonPos = Vector2((mLoadGameMenu->GetSize().x - buttonSize.x) / 2, mLoadGameMenu->GetSize().y - 0.056f * virtualHeight);
    name = "VOLTAR";
    mLoadGameMenu->AddButton(name, buttonPos,
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() {
            mLoadGameMenu->Close();
        });
}


void Game::LoadPauseMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mPauseMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(virtualWidth * 0.22f, 0.046f * virtualHeight);
    mPauseMenu->SetSize(Vector2(virtualWidth / 3, virtualHeight / 3));
    mPauseMenu->SetPosition(Vector2(virtualWidth / 3, 5 * virtualHeight / 12));
    Vector2 buttonPos = Vector2((mPauseMenu->GetSize().x - buttonSize.x) / 2, 0.0f);
    float distanceBetweenButtons = 0.064f * virtualHeight;

    auto* background = mPauseMenu->AddImage("../Assets/Sprites/Menus/FundoPreto.png", mPauseMenu->GetSize() / 2, Vector2(virtualWidth, virtualHeight) * 1.5f);
    background->SetAlpha(0.5f);

    std::string name = "CONTINUAR";
    int buttonPointSize = static_cast<int>(0.031f * virtualHeight);
    mPauseMenu->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        TogglePause();
        mPauseMenu->Close();
    });

    name = "OPÇÕES";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        LoadOptionsMenu();
    });

    name = "SELECIONAR FASE";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 2 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        LoadLevelSelectMenu();

        if (mStore->StoreOpened()) {
            mStore->CloseStore();
        }
        if (mStore->StoreMessageOpened()) {
            mStore->CloseStoreMessage();
        }
    });

    name = "VOLTAR AO MENU";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 3 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mPauseMenu->SetIsVisible(false);
        LoadConfirmBackToMenu();
    });
}

void Game::LoadConfirmBackToMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mConfirmBackToMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(virtualWidth * 0.22f, 50);
    mConfirmBackToMenu->SetSize(Vector2(virtualWidth / 3, virtualHeight / 3));
    mConfirmBackToMenu->SetPosition(Vector2(virtualWidth / 3, 5 * virtualHeight / 12));
    Vector2 buttonPos = Vector2((mConfirmBackToMenu->GetSize().x - buttonSize.x) / 2, mConfirmBackToMenu->GetSize().y * 0.30f);

    auto* background = mConfirmBackToMenu->AddImage("../Assets/Sprites/Menus/FundoPreto.png", mConfirmBackToMenu->GetSize() / 2, Vector2(virtualWidth, virtualHeight) * 1.5f);
    background->SetAlpha(0.5f);

    UIText* text = mConfirmBackToMenu->AddText("VOLTAR AO MENU?", Vector2::Zero, Vector2::Zero, 38 * mScale);
    text->SetPosition(Vector2(mConfirmBackToMenu->GetSize().x / 2, 0.0f));

    std::string name = "SIM";
    int buttonPointSize = static_cast<int>(34);
    mConfirmBackToMenu->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        SaveGame();
        SetGameScene(GameScene::MainMenu, 0.5f);
        mConfirmBackToMenu->Close();
        mPauseMenu->Close();
        if (mStore->StoreOpened()) {
            mStore->CloseStore();
        }
    });

    name = "NÃO";
    mConfirmBackToMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mConfirmBackToMenu->Close();
        mPauseMenu->SetIsVisible(true);
    });
}

void Game::LoadLevelSelectMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mLevelSelectMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mLevelSelectMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mLevelSelectMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));

    const auto buttonSize = Vector2(mLevelSelectMenu->GetSize().x * 0.8f, 50);
    const auto buttonPointSize = static_cast<int>(34);
    const auto buttonPos = Vector2(mLevelSelectMenu->GetSize().x * 0.1f, 0.0f);

    mLevelSelectMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mLevelSelectMenu->GetSize() / 2, mLevelSelectMenu->GetSize());

    std::string name = "   PRÓLOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Prologue, 0.5f);
        });

    name = "   1 - FLORESTA";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 4 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Level1, 0.5f);
        });

    name = "   2 - FOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 6 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Level2, 0.5f);
        });

    name = "   3 - PÂNTANO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 8 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Level3, 0.5f);
        });

    name = "   4 - NEVE";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 10 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Level4, 0.5f);
        });

    name = "   5 - FINAL";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 12 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Level5, 0.5f);
        });

    name = "   TESTE";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 14 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::LevelTeste, 0.5f);
        });

    name = "   COLISEU";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 16 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Coliseu, 0.5f);
        });

    name = "   ROOM 0";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 18 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            SetGameScene(GameScene::Room0, 0.5f);
        });

    name = "VOLTAR";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0.0f, mLevelSelectMenu->GetSize().y - buttonSize.y * 1.2f),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() { mLevelSelectMenu->Close(); });
}

void Game::LoadOptionsMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mOptionsMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mOptionsMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mOptionsMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));
    Vector2 buttonSize = Vector2(mOptionsMenu->GetSize().x * 0.8f, 50);
    Vector2 buttonPos = Vector2(mOptionsMenu->GetSize().x * 0.1f, 0.0f);

    mOptionsMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mOptionsMenu->GetSize() / 2, mOptionsMenu->GetSize());

    UIText* text;
    std::string name;
    int buttonPointSize = static_cast<int>(0.031f * virtualHeight);
    Vector2 textPos = Vector2(buttonSize.x * 0.05f, 0.0f);
    std::string optionValue;
    float optionPosX = mOptionsMenu->GetSize().x * 0.6f;
    UIButton* button;

    if (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        name = "FULL SCREEN";
        button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0.0f, buttonSize.y * 1.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]() {
            SDL_SetWindowFullscreen(mWindow, 0);
            mOptionsMenu->Close();
            LoadOptionsMenu();
        }, textPos);

        optionValue = "< ON >";
        text = mOptionsMenu->AddText(optionValue, Vector2::Zero, Vector2::Zero, buttonPointSize);
        text->SetPosition(Vector2(optionPosX, button->GetPosition().y + text->GetSize().y / 2));
    }
    else {
        name = "FULL SCREEN";
        button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0.0f, buttonSize.y * 1.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]() {
            Uint32 flags = SDL_GetWindowFlags(mWindow);

            // Alterna o bit de fullscreen
            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
            {
                SDL_SetWindowFullscreen(mWindow, 0);
            }
            else
            {
                SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            // int w, h;
            // SDL_GL_GetDrawableSize(mWindow, &w, &h);
            // mRenderer->OnWindowResize(static_cast<float>(w), static_cast<float>(h));
            
            mOptionsMenu->Close();
            LoadOptionsMenu();
        }, textPos);
        optionValue = "< OFF >";
        text = mOptionsMenu->AddText(optionValue, Vector2::Zero, Vector2::Zero, buttonPointSize);
        text->SetPosition(Vector2(optionPosX, button->GetPosition().y + text->GetSize().y / 2));
    }

    // name = "RESOLUÇÃO";
    // Vector2 currentRes = mResolutions[mCurrentResolutionIndex];
    // optionValue = std::to_string(static_cast<int>(currentRes.x)) + "x" + std::to_string(static_cast<int>(currentRes.y));
    //
    // button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y * 3.0f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    // [this]() {
    //     mCurrentResolutionIndex = (mCurrentResolutionIndex + 1) % mResolutions.size();
    //     Vector2 res = mResolutions[mCurrentResolutionIndex];
    //
    //     SDL_SetWindowFullscreen(mWindow, 0);  // sai do fullscreen se estiver
    //     SDL_SetWindowSize(mWindow, static_cast<int>(res.x), static_cast<int>(res.y));
    //     SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    //
    //     mOptionsMenu->Close();
    //     LoadOptionsMenu();
    // }, textPos);
    // text = mOptionsMenu->AddText(optionValue, Vector2::Zero, Vector2::Zero, buttonPointSize);
    // text->SetPosition(Vector2(optionPosX, button->GetPosition().y));

    name = "TECLADO";
    button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0.0f, buttonSize.y * 3.0f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadKeyBoardMenu2();
    }, textPos);

    name = "CONTROLE";
    button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0.0f, buttonSize.y * 4.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadControlMenu();
    }, textPos);

    name = "VOLTAR";
    mOptionsMenu->AddButton(name, buttonPos + Vector2(0.0f, mOptionsMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mOptionsMenu->Close();
    });
}

void Game::LoadControlMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mControlMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mControlMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mControlMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));
    Vector2 buttonSize = Vector2(mControlMenu->GetSize().x * 0.8f, 50);
    Vector2 buttonPos = Vector2(mControlMenu->GetSize().x * 0.1f, 0.0f);

    mControlMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mControlMenu->GetSize() / 2, mControlMenu->GetSize());
    mControlMenu->AddImage("../Assets/Sprites/Menus/Control2.png", mControlMenu->GetSize() / 2, Vector2(mControlMenu->GetSize().x, mControlMenu->GetSize().x / 1.9f));

    std::string name;
    int buttonPointSize = static_cast<int>(34);

    name = "VOLTAR";
    mControlMenu->AddButton(name, buttonPos + Vector2(0.0f, mControlMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mControlMenu->Close();
    });
}

void Game::LoadKeyBoardMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mKeyboardMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mKeyboardMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mKeyboardMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));
    Vector2 buttonSize = Vector2(mKeyboardMenu->GetSize().x * 0.8f, 50);
    Vector2 buttonPos = Vector2(mKeyboardMenu->GetSize().x * 0.1f, 0.0f);

    mKeyboardMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mKeyboardMenu->GetSize() / 2, mKeyboardMenu->GetSize());
    mKeyboardMenu->AddImage("../Assets/Sprites/Menus/Keyboard4.png", Vector2(mKeyboardMenu->GetSize().x * 0.125f, 0.0f), Vector2(mKeyboardMenu->GetSize().y * 1.4f, mKeyboardMenu->GetSize().y * 1.4f / 1.52f));

    std::string name;
    int buttonPointSize = static_cast<int>(34);


    name = "VOLTAR";
    mKeyboardMenu->AddButton(name, buttonPos + Vector2(0.0f, mKeyboardMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mKeyboardMenu->Close();
    });
}

void Game::LoadKeyBoardMenu2() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mKeyboardMenu2 = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mKeyboardMenu2->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mKeyboardMenu2->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));

    Vector2 buttonSize = Vector2(mKeyboardMenu2->GetSize().x * 0.47f, 60);
    Vector2 buttonPos = Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200);
    int buttonPointSize = static_cast<int>(40);
    Vector2 textPos = Vector2(buttonSize.x / 20, 0.0f);
    float distanceBetweenButtons = 80;

    mKeyboardMenu2->AddImage("../Assets/Sprites/Menus/Fundo2.png", mKeyboardMenu2->GetSize() / 2, mKeyboardMenu2->GetSize());

    UIText* text = mKeyboardMenu2->AddText("TECLADO", Vector2::Zero, Vector2::Zero, 50);
    text->SetPosition(Vector2(mKeyboardMenu2->GetSize().x / 2, 50));

    std::string name;

    name = "CIMA";
    mKeyboardMenu2->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[1]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[1]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[1]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 0 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[1];
        mBindingAction = Action::Up;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Up].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, text->GetSize().y / 2));

    name = "BAIXO";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 1 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[2]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[2]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[2]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 2 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[2];
        mBindingAction = Action::Down;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Down].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 1 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "PULO";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 2 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[3]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[3]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[3]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 4 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[3];
        mBindingAction = Action::Jump;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Jump].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 2 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "ATAQUE";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 3 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[4]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[4]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[4]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 6 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[4];
        mBindingAction = Action::Attack;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Attack].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 3 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "AVANÇO";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 4 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[5]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[5]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[5]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 8 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[5];
        mBindingAction = Action::Dash;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Dash].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 4 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "ATAQUE A DISTÂNCIA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 5 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[6]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[6]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[6]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 10 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[6];
        mBindingAction = Action::FireBall;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::FireBall].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 5 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "CURA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(0, 6 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[7]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[7]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[7]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(470, 12 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[7];
        mBindingAction = Action::Heal;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Heal].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(600 + text->GetSize().x / 2, 6 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "ESQUERDA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 0 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[8]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[8]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[8]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 0 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[8];
        mBindingAction = Action::MoveLeft;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::MoveLeft].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 0 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "DIREITA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 1 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[9]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[9]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[9]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 2 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[9];
        mBindingAction = Action::MoveRight;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::MoveRight].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 1 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "GANCHO";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 2 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[10]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[10]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[10]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 4 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[10];
        mBindingAction = Action::Hook;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Hook].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 2 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "ABRIR LOJA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 3 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[11]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[11]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[11]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 6 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[11];
        mBindingAction = Action::OpenStore;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::OpenStore].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 3 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "MAPA";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 4 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[12]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[12]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[12]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 8 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[12];
        mBindingAction = Action::Map;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Map].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 4 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "CAMINHAR / OLHAR";
    mKeyboardMenu2->AddButton(name, buttonPos + Vector2(mKeyboardMenu2->GetSize().x / 2, 5 * distanceBetweenButtons), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        mKeyboardMenu2->GetTexts()[13]->SetPointSize(25);
        mKeyboardMenu2->GetTexts()[13]->SetText("Pressione outra tecla");
        mKeyboardMenu2->GetTexts()[13]->SetPosition(Vector2(mKeyboardMenu2->GetSize().x * 0.01f, 200) + Vector2(mKeyboardMenu2->GetSize().x / 2 + 450, 10 * 40));
        mWaitingForKey = true;
        mNewButtonText = mKeyboardMenu2->GetTexts()[13];
        mBindingAction = Action::Look;
    }, textPos);

    text = mKeyboardMenu2->AddText(SDL_GetScancodeName(mInputBindings[Action::Look].key), Vector2::Zero, Vector2::Zero, buttonPointSize);
    text->SetPosition(buttonPos + Vector2(1300 + text->GetSize().x / 2, 5 * distanceBetweenButtons + text->GetSize().y / 2));

    name = "VOLTAR";
    mKeyboardMenu2->AddButton(name, Vector2(mKeyboardMenu2->GetSize().x * 0.375f, mKeyboardMenu2->GetSize().y * 0.9f), Vector2(mKeyboardMenu2->GetSize().x * 0.25f, 40), buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mKeyboardMenu2->Close();
    });

}


void Game::LoadObjects(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Erro ao abrir o arquivo");
        return;
    }
    nlohmann::json mapData;
    file >> mapData;
    for (const auto &layer: mapData["layers"]) {
        if (layer["name"] == "Grounds") {
            for (const auto &obj: layer["objects"]) {
                std::string name = obj["name"];
                float xOriginal = static_cast<float>(obj["x"]);
                float yOriginal = static_cast<float>(obj["y"]);
                float widthOriginal = static_cast<float>(obj["width"]);
                float heightOriginal = static_cast<float>(obj["height"]);
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                int id = obj["id"];
                bool isSpike = false;
                bool isMoving = false;
                float respawnPositionX = 0.0f;
                float respawnPositionY = 0.0f;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;
                float growSpeedX = 0.0f;
                float growSpeedY = 0.0f;
                int growthDirection = 0;
                float minHeight = 0.0f;
                float minWidth = 0.0f;
                bool isOscillating = false;
                std::string condition;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Spike") {
                            isSpike = prop["value"];
                        }
                        else if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "RespawnPositionX") {
                            respawnPositionX = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "RespawnPositionY") {
                            respawnPositionY = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                        else if (propName == "GrowSpeedX") {
                            growSpeedX = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "GrowSpeedY") {
                            growSpeedY = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "GrowthDirection") {
                            growthDirection = static_cast<int>(prop["value"]);
                        }
                        else if (propName == "MinHeight") {
                            minHeight = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "MinWidth") {
                            minWidth = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "Oscillate") {
                            isOscillating = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                if (name == "DynamicGround") {
                    auto* dynamicGround = new DynamicGround(this, minWidth, minHeight, isSpike, isMoving, movingDuration, Vector2(speedX, speedY));
                    dynamicGround->SetId(id);
                    dynamicGround->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
                    dynamicGround->SetIsOscillating(isOscillating);
                    dynamicGround->SetMaxWidth(width);
                    dynamicGround->SetMaxHeight(height);
                    dynamicGround->SetMinWidth(minWidth);
                    dynamicGround->SetMinHeight(minHeight);
                    dynamicGround->SetGrowSpeed(Vector2(growSpeedX, growSpeedY));
                    switch (growthDirection) {
                        case 0:
                            dynamicGround->SetGrowDirection(GrowthDirection::Up);
                            dynamicGround->SetPosition(Vector2(x + width / 2, y + height - minHeight / 2));
                        break;

                        case 1:
                            dynamicGround->SetGrowDirection(GrowthDirection::Down);
                            dynamicGround->SetPosition(Vector2(x + width / 2, y + minHeight / 2));
                        break;

                        case 2:
                            dynamicGround->SetGrowDirection(GrowthDirection::Left);
                            dynamicGround->SetPosition(Vector2(x + width - minWidth / 2, y + height / 2));
                        break;

                        case 3:
                            dynamicGround->SetGrowDirection(GrowthDirection::Right);
                            dynamicGround->SetPosition(Vector2(x + minWidth / 2, y + height / 2));
                        break;
                    }
                    dynamicGround->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    dynamicGround->SetTilesIndex(widthOriginal, heightOriginal, xOriginal, yOriginal);
                }
                else {
                    auto* ground = new Ground(this, width, height, isSpike, isMoving, movingDuration, Vector2(speedX, speedY));
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetTilesIndex(widthOriginal, heightOriginal, xOriginal, yOriginal);
                }
            }
        }
        if (layer["name"] == "Decorations") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                std::string imagePath;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Path") {
                            imagePath = prop["value"];
                        }
                    }
                }
                auto* decoration = new Decorations(this, width, height, imagePath);
                decoration->SetPosition(Vector2(x + width / 2, y + height / 2));
            }
        }
        if (layer["name"] == "Lava") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                float respawnPositionX = 0.0f;
                float respawnPositionY = 0.0f;
                bool isMoving = false;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "RespawnPositionX") {
                            respawnPositionX = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "RespawnPositionY") {
                            respawnPositionY = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                    }
                }
                auto* lava = new Lava(this, width, height, isMoving, movingDuration, Vector2(speedX, speedY));
                lava->SetPosition(Vector2(x + width / 2, y + height / 2));
                lava->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
            }
        }
        if (layer["name"] == "Triggers") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                bool destroy = false;
                std::string target;
                std::string event;
                std::string grounds;
                std::string enemies;
                std::vector<int> groundsIds;
                std::vector<int> enemiesIds;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                Vector2 limitMinCameraPosition(Vector2::Zero);
                Vector2 limitMaxCameraPosition(Vector2::Zero);
                std::string scene;
                int playerStartPositionId = 0;
                std::string wavePath;
                std::string worldState;
                bool worldStateFlag = false;
                std::string dialoguePath;
                std::string cutsceneId;
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Destroy") {
                            destroy = prop["value"];
                        }
                        else if (propName == "Grounds") {
                            grounds = prop["value"];
                        }
                        else if (propName == "Enemies") {
                            enemies = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionX") {
                            limitMinCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionY") {
                            limitMinCameraPosition.y = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionX") {
                            limitMaxCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionY") {
                            limitMaxCameraPosition.y = prop["value"];
                        }
                        else if (propName == "Scene") {
                            scene = prop["value"];
                        }
                        else if (propName == "PlayerStartPositionId") {
                            playerStartPositionId = prop["value"];
                        }
                        else if (propName == "Waves") {
                            wavePath = prop["value"];
                        }
                        else if (propName == "WorldState") {
                            worldState = prop["value"];
                        }
                        else if (propName == "WorldStateFlag") {
                            worldStateFlag = prop["value"];
                        }
                        else if (propName == "FilePath") {
                            dialoguePath = prop["value"];
                        }
                        else if (propName == "CutsceneId") {
                            cutsceneId = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                groundsIds = ParseIntList(grounds);
                enemiesIds = ParseIntList(enemies);

                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetDestroy(destroy);
                trigger->SetGroundsIds(groundsIds);
                trigger->SetEnemiesIds(enemiesIds);
                trigger->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                trigger->SetLimitMinCameraPosition(limitMinCameraPosition);
                trigger->SetLimitMaxCameraPosition(limitMaxCameraPosition);
                trigger->SetScene(scene);
                trigger->SetPlayerStartPositionId(playerStartPositionId);
                trigger->SetWavesPath(wavePath);
                trigger->SetWorldState(worldState);
                trigger->SetWorldStateFlag(worldStateFlag);
                trigger->SetDialoguePath(dialoguePath);
                trigger->SetCutsceneId(cutsceneId);
            }
        }
        if (layer["name"] == "Camera") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                bool destroy = false;
                std::string target;
                std::string event;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                Vector2 limitMinCameraPosition(Vector2::Zero);
                Vector2 limitMaxCameraPosition(Vector2::Zero);
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Destroy") {
                            destroy = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionX") {
                            limitMinCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionY") {
                            limitMinCameraPosition.y = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionX") {
                            limitMaxCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionY") {
                            limitMaxCameraPosition.y = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetDestroy(destroy);
                trigger->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                trigger->SetLimitMinCameraPosition(limitMinCameraPosition);
                trigger->SetLimitMaxCameraPosition(limitMaxCameraPosition);
            }
        }
        if (layer["name"] == "Levers") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                std::string target;
                std::string event;
                std::string grounds;
                std::string enemies;
                std::vector<int> groundsIds;
                std::vector<int> enemiesIds;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                std::string worldState;
                bool worldStateFlag = false;
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Grounds") {
                            grounds = prop["value"];
                        }
                        else if (propName == "Enemies") {
                            enemies = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "WorldState") {
                            worldState = prop["value"];
                        }
                        else if (propName == "WorldStateFlag") {
                            worldStateFlag = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if ((target == "DynamicGround" || target == "Ground") && !grounds.empty()) {
                    groundsIds = ParseIntList(grounds);
                }
                if (target == "Enemy") {
                    enemiesIds = ParseIntList(enemies);
                }
                auto* lever = new Lever(this);
                lever->SetPosition(Vector2(x + width / 2, y + height / 2));
                lever->SetTarget(target);
                lever->SetEvent(event);
                lever->SetGroundsIds(groundsIds);
                lever->SetEnemiesIds(enemiesIds);
                lever->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                lever->SetWorldState(worldState);
                lever->SetWorldStateFlag(worldStateFlag);

                if (!ShouldLoadObject(condition)) {
                    lever->Activate();
                }
            }
        }

        if (layer["name"] == "HookPoints") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;

                auto* hookPoint = new HookPoint(this);
                hookPoint->SetPosition(Vector2(x, y));
            }
        }

        if (layer["name"] == "SpawnPoint") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                std::string id;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "id") {
                            id = prop["value"];
                        }
                    }
                }
                AddSpawnPoint(id, Vector2(x, y));
            }
        }

        if (layer["name"] == "Enemies") {
            for (const auto &obj: layer["objects"]) {
                std::string name = obj["name"];
                int id = obj["id"];
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float MinPosX = 0;
                float MaxPosX = 0;
                float MinPosY = 0;
                float MaxPosY = 0;
                std::string grounds;
                std::vector<int> ids;
                std::string condition;
                if (name == "Enemy Simple") {
                    auto* enemySimple = new EnemySimple(this);
                    enemySimple->SetPosition(Vector2(x, y));
                    enemySimple->SetId(id);
                }
                else if (name == "Flying Enemy") {
                    auto* flyingEnemySimple = new FlyingEnemySimple(this);
                    flyingEnemySimple->SetPosition(Vector2(x, y));
                    flyingEnemySimple->SetId(id);
                }
                else if (name == "FlyingShooterEnemy") {
                    auto* flyingShooterEnemy = new FlyingShooterEnemy(this);
                    flyingShooterEnemy->SetPosition(Vector2(x, y));
                    flyingShooterEnemy->SetId(id);
                }
                else if (name == "Mantis") {
                    auto* mantis = new Mantis(this);
                    mantis->SetPosition(Vector2(x, y));
                    mantis->SetId(id);
                }
                else if (name == "FlyingGolem") {
                    auto* flyingGolem = new FlyingGolem(this);
                    flyingGolem->SetPosition(Vector2(x, y));
                    flyingGolem->SetId(id);
                }
                else if (name == "DragonFly") {
                    auto* dragonFly = new DragonFly(this);
                    dragonFly->SetPosition(Vector2(x, y));
                    dragonFly->SetId(id);
                }
                else if (name == "FlyingSpawnerEnemy") {
                    auto* flyingSpawnerEnemy = new FlyingSpawnerEnemy(this);
                    flyingSpawnerEnemy->SetPosition(Vector2(x, y));
                    flyingSpawnerEnemy->SetId(id);
                }
                else if (name == "LittleBat") {
                    auto* littleBat = new LittleBat(this);
                    littleBat->SetPosition(Vector2(x, y));
                    littleBat->SetId(id);
                }
                else if (name == "Snake") {
                    auto* snake = new Snake(this);
                    snake->SetPosition(Vector2(x, y));
                    snake->SetId(id);
                }
                else if (name == "Mushroom") {
                    auto* mushroom = new Mushroom(this);
                    mushroom->SetPosition(Vector2(x, y));
                    mushroom->SetId(id);
                }
                else if (name == "CloneEnemy") {
                    auto* cloneEnemy = new CloneEnemy(this);
                    cloneEnemy->SetPosition(Vector2(x, y));
                    cloneEnemy->SetId(id);
                }
                else if (name == "Fox") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    ids = ParseIntList(grounds);
                    auto* fox = new Fox(this);
                    fox->SetPosition(Vector2(x, y));
                    fox->SetId(id);
                }
                else if (name == "Frog") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MinPosY") {
                                MinPosY =static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "UnlockGrounds") {
                                grounds = prop["value"];
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    ids = ParseIntList(grounds);
                    auto* frog = new Frog(this);
                    frog->SetPosition(Vector2(x, y));
                    frog->SetId(id);
                    frog->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    frog->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "Moth") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* moth = new Moth(this);
                    moth->SetPosition(Vector2(x, y));
                    moth->SetId(id);
                }
                else if (name == "BushMonster") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* bushMonster = new BushMonster(this);
                    bushMonster->SetPosition(Vector2(x, y));
                    bushMonster->SetId(id);
                }
                else if (name == "Golem") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MinPosY") {
                                MinPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "UnlockGrounds") {
                                grounds = prop["value"];
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* golem = new Golem(this);
                    golem->SetPosition(Vector2(x, y));
                    golem->SetId(id);
                    golem->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    golem->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "HookEnemy") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* hookEnemy = new HookEnemy(this);
                    hookEnemy->SetPosition(Vector2(x, y));
                    hookEnemy->SetId(id);
                }
                else if (name == "MirrorBoss") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MinPosY") {
                                MinPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* mirrorBoss = new MirrorBoss(this);
                    mirrorBoss->SetPosition(Vector2(x, y));
                    mirrorBoss->SetId(id);
                    mirrorBoss->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    mirrorBoss->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
            }
        }
        if (layer["name"] == "Checkpoint") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                Vector2 cameraPosition(Vector2::Zero);

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "CameraPositionX") {
                            cameraPosition.x = prop["value"];
                        }
                        if (propName == "CameraPositionY") {
                            cameraPosition.y = prop["value"];
                        }
                    }
                }
                auto checkpoint = new Checkpoint(this, width, height, Vector2(x + width / 2, y + height / 2));
            }
        }

        if (layer["name"] == "Player") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                int playerStartPositionId = 0;
                Vector2 enteringLevelVelocity(Vector2::Zero);

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "StartPositionId") {
                            playerStartPositionId = prop["value"];
                        }
                        if (propName == "EnteringLevelSpeedX") {
                            enteringLevelVelocity.x = static_cast<float>(prop["value"]) * mScale;
                        }
                        if (propName == "EnteringLevelSpeedY") {
                            enteringLevelVelocity.y = static_cast<float>(prop["value"]) * mScale;
                        }
                    }
                }

                if (playerStartPositionId != mPlayerStartPositionId) {
                    continue;
                }

                if (mPlayer) {
                    mPlayer->SetSword();
                    mPlayer->SetJumpEffects();
                    mPlayer->InitLight();
                    mPlayer->GetComponent<DashComponent>()->InitDashEffect();
                    mPlayer->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2::Zero);
                    mPlayer->SetIsDead(false);
                    mPlayer->SetState(ActorState::Active);
                    mPlayer->SetInvertControls(false);
                    mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                    if (mGoingToNextLevel) {
                        mPlayer->SetPosition(Vector2(x, y));
                        mPlayer->SetIsEnteringLevel(enteringLevelVelocity);

                        // Salva jogo
                        SaveGame();

                        mGoingToNextLevel = false;
                    }
                    else {
                        mPlayer->SetPosition(mCheckpointPosition);
                    }

                    // Faz isso para o player ser sempre o último a ser atualizado a cada frame
                    RemoveActor(mPlayer);
                    AddActor(mPlayer);
                }
                else {
                    mPlayer = new Player(this);
                    mSaveData->ApplyToPlayer();
                    mCheckPointMoney = mPlayer->GetMoney();
                }
            }
            mPlayerStartPositionId = 0;
        }
    }
}

void Game::LoadLevel(const std::string &fileName) {
    // Abre arquivo json
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Erro ao abrir o arquivo");
        return;
    }
    nlohmann::json mapData;
    file >> mapData;

    // Lê altura, largura e tileSize
    int height = int(mapData["height"]);
    int width = int(mapData["width"]);
    float tileSize = static_cast<float>(mapData["tilewidth"]) * mScale;
    mLevelHeight = height;
    mLevelWidth = width;
    mTileSize = tileSize;

    if (mGoingToNextLevel) {
        // Delete map
        // if (mMap) {
        //     delete mMap;
        //     mMap = nullptr;
        // }
    }
    // Lê matrizes de tiles
    for (const auto& layer : mapData["layers"]) {
        if (layer["name"] == "Camada de Blocos 1") {
            std::vector<int> data = layer["data"];
            int** matrix = new int*[height];
            for (int i = 0; i < height; ++i) {
                matrix[i] = new int[width];
                for (int j = 0; j < width; ++j) {
                    matrix[i][j] = data[i * width + j];
                }
            }
            mLevelData = matrix;
        } else if (layer["name"] == "DynamicGrounds") {
            std::vector<int> data = layer["data"];
            int** matrix = new int*[height];
            for (int i = 0; i < height; ++i) {
                matrix[i] = new int[width];
                for (int j = 0; j < width; ++j) {
                    matrix[i][j] = data[i * width + j];
                }
            }
            mLevelDataDynamicGrounds = matrix;
        } else if (!mMap) {
            if (layer["name"] == "Map") {
                std::vector<int> data = layer["data"];
                int** matrix = new int*[height];
                for (int i = 0; i < height; ++i) {
                    matrix[i] = new int[width];
                    for (int j = 0; j < width; ++j) {
                        matrix[i][j] = data[i * width + j];
                    }
                }
                // mMap = new Map(this, matrix, mLevelWidth, mLevelHeight);
            }
        }
    }

    // Load tilesheet texture
    size_t pos = fileName.rfind(".json");
    std::string tileSheetTexturePath = fileName.substr(0, pos) + ".png";
    // mTileSheet = LoadTexture(tileSheetTexturePath);
    mTileSheet = mRenderer->GetTexture(tileSheetTexturePath);


    // Load tilesheet data
    std::string tileSheetDataPath = fileName.substr(0, pos) + "TileSet.json";
    std::ifstream tileSheetFile(tileSheetDataPath);

    nlohmann::json tileSheetData = nlohmann::json::parse(tileSheetFile);

    int textureWidth = mTileSheet->GetWidth();
    int textureHeight = mTileSheet->GetHeight();

    for (const auto &tile: tileSheetData["sprites"]) {
        std::string tileFileName = tile["fileName"];
        int x = tile["x"].get<int>();
        int y = tile["y"].get<int>();
        int w = tile["width"].get<int>();
        int h = tile["height"].get<int>();

        size_t dotPos = tileFileName.find('.');
        std::string numberStr = tileFileName.substr(0, dotPos);
        int index = std::stoi(numberStr);

        // Normaliza para [0, 1]
        float u = static_cast<float>(x) / textureWidth;
        float v = static_cast<float>(y) / textureHeight;
        float uw = static_cast<float>(w) / textureWidth;
        float vh = static_cast<float>(h) / textureHeight;

        mTileSheetData[index] = Vector4(u, v, uw, vh);
    }

    // Cria objetos
    LoadObjects(fileName);
}

bool Game::ShouldLoadObject(const std::string &condition) {
    if (condition.empty()) return true;

    bool negate = condition[0] == '!';
    std::string flag = negate ? condition.substr(1) : condition;

    bool value = mWorldState[flag];
    return negate ? !value : value;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                    event.window.event == SDL_WINDOWEVENT_RESIZED) 
                {
                    float oldScale = mScale;
                    mWindowWidth = event.window.data1;
                    mWindowHeight = event.window.data2;

                    // Pega o NOVO tamanho em PIXELS
                    int newWidth, newHeight;
                    SDL_GL_GetDrawableSize(mWindow, &newWidth, &newHeight);
                    
                    // Avisa o Renderer
                    if (mRenderer)
                    {
                        mRenderer->OnWindowResize(static_cast<float>(newWidth), 
                                                static_cast<float>(newHeight));
                    }

                    mLogicalWindowWidth = mRenderer->GetVirtualWidth();
                    mLogicalWindowHeight = mRenderer->GetVirtualHeight();
                    // if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
                    //     // Comenta essa parte para tirar o zoom do mapa
                    //     mLogicalWindowWidth = static_cast<float>(mWindowWidth);
                    //     mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
                    //     SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
                    //
                    //     float ratio = mOriginalWindowWidth / static_cast<float>(mLogicalWindowWidth);
                    //     mScale = 1 / ratio;
                    //     // int tileSize = static_cast<int>(mOriginalTileSize / ratio);
                    //     // mScale = static_cast<float>(tileSize) / mOriginalTileSize;
                    // }
                    // else {
                    //     // Comenta essa parte para tirar o zoom do mapa
                    //     mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
                    //     mLogicalWindowHeight = static_cast<float>(mWindowHeight);
                    //     SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
                    //
                    //     float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
                    //     mScale = 1 / ratio;
                    //     // int tileSize = static_cast<int>(mOriginalTileSize / ratio);
                    //     // mScale = static_cast<float>(tileSize) / mOriginalTileSize;
                    // }
                    // const float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
                    // const int tileSize = static_cast<int>(32 / ratio);
                    // mScale = static_cast<float>(tileSize) / 32.0f;
                    // ChangeResolution(oldScale);
                }
                break;

            case SDL_KEYDOWN:
                if (mWaitingForKey) {
                    SDL_Scancode sc = event.key.keysym.scancode;

                    // converte scancode para string legível
                    std::string keyName = SDL_GetScancodeName(sc);

                    // atualiza o texto do botão
                    if (mNewButtonText) {
                        mNewButtonText->SetPointSize(40 * mScale);
                        mNewButtonText->SetText(keyName);
                        mNewButtonText->SetPosition(mNewButtonText->GetPosition() + Vector2(50, 0) * mScale);
                    }

                    mInputBindings[mBindingAction].key = sc;

                    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");

                    // sai do modo de captura
                    mWaitingForKey = false;
                    mNewButtonText = nullptr;
                }

                else if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = true;
                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym, SDL_CONTROLLER_BUTTON_INVALID, 0, 0);
                    }

                    // if (event.key.keysym.sym == SDLK_ESCAPE) {
                    //     if (!mStore->StoreOpened() && mGameScene != GameScene::MainMenu) {
                    //         TogglePause();
                    //         if (mIsPaused) {
                    //             LoadPauseMenu();
                    //         }
                    //         else {
                    //             mPauseMenu->Close();
                    //         }
                    //     }
                    // }

                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        // if (!mShowMap &&
                        //     mGameScene != GameScene::MainMenu &&
                        //     mGamePlayState != GamePlayState::Cutscene)
                        if (mGameScene != GameScene::MainMenu &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); *iter != mHUD; ++iter) {
                                    if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                    }
                                }
                                if (mStore->StoreMessageOpened()) {
                                    mStore->CloseStoreMessage();
                                }
                                if (mStore->StoreOpened()) {
                                    mStore->CloseStore();
                                }
                                else {
                                    TogglePause();
                                }
                            }
                            else {
                                TogglePause();
                                LoadPauseMenu();
                            }
                        }
                        else if (mGameScene == GameScene::MainMenu) {
                            for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend() - 1; ++iter) {
                                if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                    (*iter)->Close();
                                }
                            }
                        }
                    }

                    if (SDL_GetScancodeFromKey(event.key.keysym.sym) == mInputBindings[Action::Map].key) {
                    // if (event.key.keysym.sym == SDLK_m) {
                        // if (mMap) {
                        //     if (!mIsPaused || (mIsPaused && mShowMap)) {
                        //         mShowMap = !mShowMap;
                        //         // TogglePause();
                        //     }
                        // }
                    }

                    if (event.key.keysym.sym == SDLK_8) {
                        Quit();
                    }

                    if (event.key.keysym.sym == SDLK_5) {
                        mIsSlowMotion = !mIsSlowMotion;
                        mIsAccelerated = false;
                    }

                    if (event.key.keysym.sym == SDLK_6) {
                        mIsAccelerated = !mIsAccelerated;
                        mIsSlowMotion = false;
                    }

                    if (event.key.keysym.sym == SDLK_1) {
                        mCamera->SetZoom(2.0f);
                    }
                    if (event.key.keysym.sym == SDLK_2) {
                        mCamera->SetZoom(0.5f);
                    }
                    if (event.key.keysym.sym == SDLK_3) {
                        mCamera->SetZoom(1.0f);
                    }
                }
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                if (mWaitingForKey) {
                    break;
                }

                if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = false;

                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, event.cbutton.button, 0, 0);
                    }

                    // if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                    //     if (!mStore->StoreOpened() && mGameScene != GameScene::MainMenu) {
                    //         TogglePause();
                    //         if (mIsPaused) {
                    //             LoadPauseMenu();
                    //         }
                    //         else {
                    //             mPauseMenu->Close();
                    //         }
                    //     }
                    // }

                    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                        // if (!mShowMap &&
                        //     mGameScene != GameScene::MainMenu &&
                        //     mGamePlayState != GamePlayState::Cutscene)
                        if (mGameScene != GameScene::MainMenu &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); *iter != mHUD; ++iter) {
                                    if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                    }
                                }
                                if (mStore->StoreMessageOpened()) {
                                    mStore->CloseStoreMessage();
                                }
                                if (mStore->StoreOpened()) {
                                    mStore->CloseStore();
                                }
                                else {
                                    TogglePause();
                                }
                            }
                            else {
                                TogglePause();
                                LoadPauseMenu();
                            }
                        }
                        else if (mGameScene == GameScene::MainMenu) {
                            for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend() - 1; ++iter) {
                                if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                    (*iter)->Close();
                                }
                            }
                        }
                    }

                    // Apertar B para sair dos menus
                    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
                        if (!mShowMap &&
                            mGameScene != GameScene::MainMenu &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); *iter != mHUD; ++iter) {
                                    if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                    }
                                }
                                if (mStore->StoreMessageOpened()) {
                                    mStore->CloseStoreMessage();
                                }
                                if (mStore->StoreOpened()) {
                                    mStore->CloseStore();
                                }
                                else {
                                    TogglePause();
                                }
                            }
                        }
                        else if (mGameScene == GameScene::MainMenu) {
                            for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend() - 1; ++iter) {
                                if ((*iter)->GetState() != UIScreen::UIState::Closing) {
                                    (*iter)->Close();
                                }
                            }
                        }
                    }

                    if (event.cbutton.button == mInputBindings[Action::Map].btn) {
                        // if (mMap) {
                        //     if (!mIsPaused || (mIsPaused && mShowMap)) {
                        //         mShowMap = !mShowMap;
                        //         // TogglePause();
                        //     }
                        // }
                    }
                }
                break;

            case SDL_CONTROLLERAXISMOTION:
                if (mWaitingForKey) {
                    break;
                }

                if (mGamePlayState != GamePlayState::GameOver) {
                    if (Math::Abs(event.caxis.value) > DEAD_ZONE) {
                        mIsPlayingOnKeyboard = false;
                    }

                    if (!mUIStack.empty()) {
                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                            int valueY = event.caxis.value;

                            if (valueY < -DEAD_ZONE) {
                                if (mLeftStickStateY != StickState::Up) {
                                    mLeftStickStateY = StickState::Up;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, valueY, 0);
                                    }
                                }
                            }
                            else if (valueY > DEAD_ZONE) {
                                if (mLeftStickStateY != StickState::Down) {
                                    mLeftStickStateY = StickState::Down;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, valueY, 0);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                mLeftStickStateY = StickState::Neutral;
                            }
                        }

                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                            int valueX = event.caxis.value;

                            if (valueX < -DEAD_ZONE) {
                                if (mLeftStickStateX != StickState::Left) {
                                    mLeftStickStateX = StickState::Left;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, 0, valueX);
                                    }
                                }
                            }
                            else if (valueX > DEAD_ZONE) {
                                if (mLeftStickStateX != StickState::Right) {
                                    mLeftStickStateX = StickState::Right;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, 0, valueX);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                mLeftStickStateX = StickState::Neutral;
                            }
                        }
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (mWaitingForKey) {
                    break;
                }

                if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = true;

                    // Handle mouse for UI screens
                    if (!mUIStack.empty()) {
                        // 1. Obter coordenadas da tela (física)
                        Vector2 screenPos(static_cast<float>(event.button.x), static_cast<float>(event.button.y));

                        // 2. Converter para coordenadas virtuais
                        Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

                        // 3. Passar as coordenadas limpas para a UI
                        mUIStack.back()->HandleMousePress(virtualPos);
                    }
                }
                break;

            case SDL_MOUSEMOTION:
                if (mWaitingForKey) {
                    break;
                }
                mIsPlayingOnKeyboard = true;

                // Handle mouse for UI screens
                if (!mUIStack.empty()) {
                    // 1. Obter coordenadas da tela (física)
                    Vector2 screenPos(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));

                    // 2. Converter para coordenadas virtuais
                    Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

                    // 3. Passar as coordenadas limpas para a UI
                    mUIStack.back()->HandleMouseMotion(virtualPos);
                }
                break;

            case SDL_CONTROLLERDEVICEADDED:
                mController = SDL_GameControllerOpen(event.cdevice.which);
                // if (mController) {
                //     SDL_Log("Controle conectado!");
                // }
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                if (mController) {
                    if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mController)) == event.cdevice.which) {
                        SDL_GameControllerClose(mController);
                        mController = nullptr;
                        // SDL_Log("Controle removido!");
                    }
                }
                break;

            default:
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mGamePlayState == GamePlayState::Playing &&
        !mGoingToNextLevel) {
        if (!mIsPaused) {
            if (mHitstopActive) {}
            else {
                for (auto actor: mActors) {
                    actor->ProcessInput(state, *mController);
                }
            }
        }
    }
}

void Game::TogglePause() {
    if (mGameScene != GameScene::MainMenu &&
        mGamePlayState != GamePlayState::Cutscene)
    {
        mIsPaused = !mIsPaused;
        if (mIsPaused) {
            if (mAudio->GetSoundState(mMusicHandle) == SoundState::Playing) {
                mAudio->PauseSound(mMusicHandle);
            }
            if (mAudio->GetSoundState(mBossMusic) == SoundState::Playing) {
                mAudio->PauseSound(mBossMusic);
            }
            mGamePlayState = GamePlayState::Paused;
        }
        else {
            if (mAudio->GetSoundState(mBossMusic) == SoundState::Paused) {
                mAudio->ResumeSound(mBossMusic);
            }
            else if (mAudio->GetSoundState(mMusicHandle) == SoundState::Paused) {
                mAudio->ResumeSound(mMusicHandle);
            }
            mGamePlayState = GamePlayState::Playing;
        }
        mPlayer->SetCanJump(false);
        mPlayer->SetPrevFireBallPressed(true);
    }
}


void Game::UpdateGame()
{
    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 1000.0 / mFPS));

    const auto frameDuration = static_cast<Uint32>(1000.0f / mFPS);
    Uint32 now = SDL_GetTicks();
    if (now < mTicksCount + frameDuration)
        SDL_Delay((mTicksCount + frameDuration) - now);

    float deltaTime = static_cast<float>(SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if (mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::Menu) {
        mTotalPlayTime += deltaTime;
    }

    // testes para alterar velocidade do jogo
    if (mIsSlowMotion) {
        deltaTime *= 0.5;
    }
    if (mIsAccelerated) {
        deltaTime *= 1.5;
    }

    // if (mMap && mGamePlayState == GamePlayState::Playing) {
    //     mMap->Update(deltaTime);
    // }

    // SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255); // Usado para deixar as bordas em preto
    // SDL_RenderClear(mRenderer);

    // Update all actors and pending actors
    if (!mIsPaused) {
        if (mHitstopDelayActive) {
            if (mHitstopDelayTimer < mHitstopDelayDuration) {
                mHitstopDelayTimer += deltaTime;
            }
            else {
                mHitstopDelayActive = false;
                mHitstopActive = true;
            }
        }
        if (mHitstopActive) {
            if (mHitstopTimer < mHitstopDuration) {
                mHitstopTimer += deltaTime;
            }
            else {
                mHitstopActive = false;
            }
        }
        else {
            UpdateActors(deltaTime);
            if (mWaveManager) {
                mWaveManager->Update(deltaTime);
            }
            // if (mHUD) {
            //     mHUD->Update(deltaTime);
            // }
        }
    }

    if (mIsPlayingFinalCutscene) {
        PlayFinalGoodCutscene();
    }

    // Update cutscene
    if (mCurrentCutscene) {
        mCurrentCutscene->Update(deltaTime);
        if (mCurrentCutscene->IsComplete()) {
            delete mCurrentCutscene;          // libera memória manualmente
            mCurrentCutscene = nullptr;       // evita dangling pointer
            SetGamePlayState(Game::GamePlayState::Playing);
        }
    }

    if (mGamePlayState == GamePlayState::Cutscene || mGamePlayState == GamePlayState::GameOver) {
        if (mHUD) {
            mHUD->SetIsVisible(false);
        }
    }
    else {
        if (mHUD) {
            mHUD->SetIsVisible(true);
        }
    }

    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        // if (ui != mHUD) {
            if (ui->GetState() == UIScreen::UIState::Active) {
                ui->Update(deltaTime);
            }
        // }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            *iter = nullptr;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    if (mGamePlayState == GamePlayState::Playing ||
        mGamePlayState == GamePlayState::LevelComplete ||
        mGamePlayState == GamePlayState::Cutscene)
    {
        // Esconde o cursor
        SDL_ShowCursor(SDL_DISABLE);
    }
    else {
        SDL_ShowCursor(SDL_ENABLE);
    }

    if (mIsCrossFading) {
        if (mCrossFadeTimer < mCrossFadeDuration) {
            if (mCrossFadeTimer < mCrossFadeDuration * 0.4f) {
                float progress = mCrossFadeTimer / (mCrossFadeDuration * 0.4f);
                mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
            }
            else if (mCrossFadeTimer >= mCrossFadeDuration * 0.4f && mCrossFadeTimer < mCrossFadeDuration * 0.6f) {
                mFadeAlpha = 255;
                if (mHitByLava) {
                    mPlayer->SetPosition(mLavaRespawnPosition);
                    mPlayer->SetState(ActorState::Active);
                    mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                }
            }
            else if (mCrossFadeTimer >= mCrossFadeDuration * 0.6f) {
                float progress = 1 - (mCrossFadeTimer - (mCrossFadeDuration * 0.6f)) / (mCrossFadeDuration * 0.4f);
                mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
            }
            mCrossFadeTimer += deltaTime;
        }
        else {
            mIsCrossFading = false;
            if (mHitByLava) {
                mPlayer->SetState(ActorState::Active);
                mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                mHitByLava = false;
            }
        }
    }

    if (mBackToCheckpoint) {
        mStore->CloseStoreMessage();
        SetGameScene(mCheckpointGameScene, 1.5f);
        mPlayer->ResetHealthPoints();
        mPlayer->ResetMana();
        mPlayer->ResetHealCount();
        mPlayer->SetMoney(mCheckPointMoney);
        mBackToCheckpoint = false;
    }

    UpdateCamera(deltaTime);

    UpdateSceneManager(deltaTime);
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;

        // Cálculo proporcional da opacidade
        if (mSceneManagerTimer <= mFadeDuration) {
            float progress = 1.0f - (mSceneManagerTimer / mFadeDuration);
            mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
        }

        if (mSceneManagerTimer <= 0.0f) {
            // mSceneManagerTimer = TRANSITION_TIME;  // Reinicia timer para próxima fase
            mSceneManagerTimer = mTransitionTime;  // Reinicia timer para próxima fase
            mSceneManagerState = SceneManagerState::Active;
            mFadeAlpha = 0;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f) {
            ChangeScene();  // Realiza a troca de cena
            mSceneManagerTimer = mFadeDuration;
            mSceneManagerState = SceneManagerState::Exiting;
        }
    }

    if (mSceneManagerState == SceneManagerState::Exiting) {
        mSceneManagerTimer -= deltaTime;

        // Cálculo proporcional da opacidade
        float progress = mSceneManagerTimer / mFadeDuration;
        mFadeAlpha = static_cast<Uint8>(progress * 255.0f);

        if (mSceneManagerTimer <= 0.0f) {
            // mSceneManagerTimer = TRANSITION_TIME;  // Reinicia timer para próxima fase
            mSceneManagerTimer = mTransitionTime;  // Reinicia timer para próxima fase
            mSceneManagerState = SceneManagerState::None;
            mFadeAlpha = 0;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor: mActors)
        actor->Update(deltaTime);

    mUpdatingActors = false;

    for (auto pending: mPendingActors)
        mActors.emplace_back(pending);

    mPendingActors.clear();

    std::vector<Actor* > deadActors;
    for (auto actor: mActors)
        if (actor->GetState() == ActorState::Destroy)
            deadActors.emplace_back(actor);

    for (auto actor: deadActors)
        delete actor;
}

void Game::UpdateCamera(float deltaTime) {
    if (!mCamera) {
        return;
    }
    mCamera->Update(deltaTime);
}

void Game::AddGround(class Ground* g) { mGrounds.emplace_back(g); }

void Game::RemoveGround(class Ground* g) {
    auto iter = std::find(mGrounds.begin(), mGrounds.end(), g);
    if (iter != mGrounds.end()) {
        mGrounds.erase(iter);
    }
}

Ground* Game::GetGroundById(int id) {
    for (Ground* g : mGrounds) {
        if (g->GetId() == id) {
            return g;
        }
    }
    return nullptr;
}

void Game::AddFireBall(class FireBall* f) { mFireBalls.emplace_back(f); }

void Game::RemoveFireball(class FireBall* f) {
    auto iter = std::find(mFireBalls.begin(), mFireBalls.end(), f);
    if (iter != mFireBalls.end()) {
        mFireBalls.erase(iter);
    }
}

void Game::AddParticle(class Particle* p) { mParticles.emplace_back(p); }

void Game::RemoveParticle(class Particle* p) {
    auto iter = std::find(mParticles.begin(), mParticles.end(), p);
    if (iter != mParticles.end()) {
        mParticles.erase(iter);
    }
}

void Game::AddProjectile(class Projectile* p) { mProjectiles.emplace_back(p); }

void Game::RemoveProjectile(class Projectile* p) {
    auto iter = std::find(mProjectiles.begin(), mProjectiles.end(), p);
    if (iter != mProjectiles.end()) {
        mProjectiles.erase(iter);
    }
}

void Game::AddMoney(class Money* m) { mMoneys.emplace_back(m); }

void Game::RemoveMoney(class Money* m) {
    auto iter = std::find(mMoneys.begin(), mMoneys.end(), m);
    if (iter != mMoneys.end()) {
        mMoneys.erase(iter);
    }
}

void Game::AddHookPoint(class HookPoint* hp) { mHookPoints.emplace_back(hp); }

void Game::RemoveHookPoint(class HookPoint* hp) {
    auto iter = std::find(mHookPoints.begin(), mHookPoints.end(), hp);
    if (iter != mHookPoints.end()) {
        mHookPoints.erase(iter);
    }
}

void Game::AddSpawnPoint(const std::string &id, const Vector2 &pos) {
    mSpawnPoints[id] = pos;
}

Vector2 Game::GetSpawnPointPosition(const std::string &id) const {
    auto it = mSpawnPoints.find(id);
    if (it != mSpawnPoints.end()) {
        return it->second;
    }
    throw std::runtime_error("SpawnPoint não encontrado: " + id);
}

std::vector<Vector2> Game::GetSpawnPointsPositions() {
    std::vector<Vector2> positions;

    for (const auto& par : mSpawnPoints) {
        positions.emplace_back(par.second);
    }

    return positions;
}

void Game::CreateWaveManager(std::string wavesFilePath) {
    if (!mWaveManager) {
        mWaveManager = new WaveManager(this);
        mWaveManager->LoadFromJson(wavesFilePath);
        mWaveManager->Start();
    }
}


void Game::AddEnemy(class Enemy* e) { mEnemies.emplace_back(e); }

void Game::RemoveEnemy(class Enemy* e) {
    auto iter = std::find(mEnemies.begin(), mEnemies.end(), e);
    if (iter != mEnemies.end()) {
        mEnemies.erase(iter);
    }
}

Enemy* Game::GetEnemyById(int id) {
    for (Enemy* e : mEnemies) {
        if (e->GetId() == id) {
            return e;
        }
    }
    return nullptr;
}

void Game::AddActor(Actor* actor) {
    if (mUpdatingActors) {
        mPendingActors.emplace_back(actor);
    }
    else {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor) {
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::InitCrossFade(float duration) {
    mIsCrossFading = true;
    mCrossFadeTimer = 0;
    mCrossFadeDuration = duration;
}


void Game::StartBossMusic(SoundHandle music) {
    mBossMusic = music;
    mAudio->PauseSound(mMusicHandle);
}

void Game::StopBossMusic() {
    mAudio->StopSound(mBossMusic);
    mBossMusic.Reset();
    mAudio->ResumeSound(mMusicHandle);
}

void Game::SaveGame() {
    mSaveData->CaptureFromGame();
    mSaveManager->SaveGame(mSaveData, mSaveSlot);
}

void Game::LoadGame() {
    mSaveData = mSaveManager->LoadGame(mSaveSlot);
    mSaveData->ApplyToGame();
    mSaveData->ApplyWorldState();
}

void Game::SetWorldFlag(const std::string &key, bool value) {
    mWorldState[key] = value;
}

bool Game::GetWorldFlag(const std::string &key) const {
    auto it = mWorldState.find(key);
    if (it != mWorldState.end())
        return it->second;
    return false; // padrão se não existe
}

void Game::PlayFinalGoodCutscene() {
    if (mCutsceneIndex > 0) {
        mPlayer->SetPosition(Vector2(-100, -100));
    }
    if (mCutsceneIndex < mGoodCutscenes.size()) {
        mGamePlayState = GamePlayState::Cutscene;
        if (mCurrentCutscene == nullptr) {
            if (mGameScene != mGoodCutsceneScenes[mCutsceneIndex]) {
                SetGameScene(mGoodCutsceneScenes[mCutsceneIndex], 1.5f);
            }
            else {
                mCurrentCutscene = new Cutscene(this, mGoodCutscenes[mCutsceneIndex], "../Assets/Cutscenes/Cutscenes.json");
                mCutsceneIndex++;
            }
        }
    }
    else {
        if (mCurrentCutscene == nullptr) {
            SetGameScene(GameScene::MainMenu, 1.5f);
            // mGoingToNextLevel = true;
            mIsPlayingFinalCutscene = false;
        }
    }
}

void Game::PlayFinalEvilCutscene() {

}

std::string Game::ActionToString(Action action) {
    switch (action) {
        case Action::MoveLeft:  return "MoveLeft";
        case Action::MoveRight: return "MoveRight";
        case Action::Up:        return "Up";
        case Action::Down:      return "Down";
        case Action::Look:      return "Look";
        case Action::Jump:      return "Jump";
        case Action::Dash:      return "Dash";
        case Action::Attack:    return "Attack";
        case Action::FireBall:  return "FireBall";
        case Action::Heal:      return "Heal";
        case Action::Hook:      return "Hook";
        case Action::Pause:     return "Pause";
        case Action::OpenStore: return "OpenStore";
        case Action::Confirm:   return "Confirm";
        case Action::Map:       return "Map";
        default:                return "Unknown";
    }
}

Game::Action Game::StringToAction(const std::string &str) {
    if (str == "MoveLeft")  return Action::MoveLeft;
    if (str == "MoveRight") return Action::MoveRight;
    if (str == "Up")        return Action::Up;
    if (str == "Down")      return Action::Down;
    if (str == "Look")      return Action::Look;
    if (str == "Jump")      return Action::Jump;
    if (str == "Dash")      return Action::Dash;
    if (str == "Attack")    return Action::Attack;
    if (str == "FireBall")  return Action::FireBall;
    if (str == "Heal")      return Action::Heal;
    if (str == "Hook")      return Action::Hook;
    if (str == "Pause")     return Action::Pause;
    if (str == "OpenStore") return Action::OpenStore;
    if (str == "Confirm")   return Action::Confirm;
    if (str == "Map")       return Action::Map;

    return Action::Confirm;
}

void Game::SaveBindingsToFile(const std::string &filename) {
    nlohmann::json j;

    for (const auto& pair : mInputBindings) {
        Action action = pair.first;
        InputBinding binding = pair.second;

        // Cria um objeto JSON para o binding atual
        nlohmann::json bindingJson;
        bindingJson["key"] = static_cast<int>(binding.key);
        bindingJson["btn"] = static_cast<int>(binding.btn);

        // Usa a string da Action como chave no JSON principal
        j[ActionToString(action)] = bindingJson;
    }

    // Abre o arquivo e escreve o JSON formatado
    std::ofstream file(filename);
    if (file.is_open()) {
        // j.dump(4) formata o JSON com 4 espaços de indentação para ficar legível
        file << j.dump(4);
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para salvar os controles: " << filename << std::endl;
    }
}

void Game::LoadBindingsFromFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Arquivo de controles '" << filename << "' nao encontrado. Criando controles padrao." << std::endl;

        // --- Crie aqui seus controles padrão ---
        mInputBindings[Action::MoveLeft]  = {SDL_SCANCODE_A, SDL_CONTROLLER_BUTTON_DPAD_LEFT};
        mInputBindings[Action::MoveRight] = {SDL_SCANCODE_D, SDL_CONTROLLER_BUTTON_DPAD_RIGHT};
        mInputBindings[Action::Jump]      = {SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A};
        mInputBindings[Action::Attack]    = {SDL_SCANCODE_J, SDL_CONTROLLER_BUTTON_X};
        // ... adicione todos os outros padrões

        // Salva os padrões para que o arquivo exista na próxima vez
        SaveBindingsToFile(filename);
        return;
    }

    try {
        nlohmann::json j;
        file >> j;

        mInputBindings.clear(); // Limpa os bindings antigos antes de carregar os novos

        // Itera sobre todos os elementos do JSON
        for (auto& [key_str, value] : j.items()) {
            Action action = StringToAction(key_str);

            SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
            SDL_GameControllerButton button = SDL_CONTROLLER_BUTTON_INVALID;

            // Verifica se as chaves "key" e "btn" existem antes de tentar acessá-las
            if (value.contains("key") && value["key"].is_number()) {
                scancode = static_cast<SDL_Scancode>(value["key"].get<int>());
            }
            if (value.contains("btn") && value["btn"].is_number()) {
                button = static_cast<SDL_GameControllerButton>(value["btn"].get<int>());
            }

            mInputBindings[action] = {scancode, button};
        }

    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Erro de parse no JSON: " << e.what() << '\n'
                  << "ID da excecao: " << e.id << '\n'
                  << "Byte do erro: " << e.byte << std::endl;
    }
}

bool Game::IsActionPressed(Action action, const Uint8 *keyboardState, SDL_GameController *controller) {
    auto& binding = mInputBindings[action];

    // Teclado
    if (keyboardState[binding.key]) return true;

    // Controle
    if (controller && SDL_GameControllerGetButton(controller, binding.btn)) return true;

    return false;
}


void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    mRenderer->BeginGameDraw();

    if (mCamera) {
        if (mUseParallaxBackground) {
            switch (mGameScene) {
                case GameScene::LevelTeste:
                    // DrawParallaxLayers(mBackgroundLayersLevel3);
                break;

                case GameScene::Coliseu:
                    DrawParallaxLayers(mBackgroundLayersLevel2);
                break;

                case GameScene::Level1:
                    // DrawParallaxLayers(mBackgroundLayersLevel3);
                    break;

                case GameScene::Level2:
                    // DrawParallaxLayers(mBackgroundLayersLevel2);
                    break;

                case GameScene::Level3:
                    // DrawParallaxLayers(mBackgroundLayersLevel3);
                    break;

                case GameScene::Level4:
                    // DrawParallaxLayers(mBackgroundLayersLevel4);
                    break;

                case GameScene::Level5:
                    // DrawParallaxLayers(mBackgroundLayersLevel4);
                    break;

                case GameScene::Room0:
                    DrawParallaxLayers(mBackgroundLayersLevel2);
                    break;

                case GameScene::MirrorBoss:
                    DrawParallaxLayers(mBackgroundLayersLevel2);
                    break;

                default:
                    break;
            }
        }
        else {
            // DrawParallaxBackground(mBackGroundTexture); // desenha o fundo com repetição horizontal
            mRenderer->DrawTexture(Vector2(33000, 3200),
                       Vector2(66000, 6400), 0.0f, Color::White,
                       mBackGroundTexture, Vector4::UnitRect, mCamera->GetPosCamera());
        }
        //     // Ordem de desenho: mais distantes primeiro
        //     // DrawParallaxLayer(mSky,        0.1f, 0, mWindowHeight / 2);  // camada mais distante
        //     // DrawParallaxLayer(mMountains,  0.3f, mWindowHeight / 4, mWindowHeight / 3);  // montanhas ao fundo
        //     // DrawParallaxLayer(mTreesBack,  0.5f, mWindowHeight / 3, mWindowHeight / 2);  // árvores distantes
        //     // DrawParallaxLayer(mTreesFront, 0.7f, mWindowHeight / 2, mWindowHeight / 2);  // árvores próximas
    }
    else {
        mRenderer->DrawTexture(Vector2(mWindowWidth / 2, mWindowHeight / 2),
                               Vector2(mWindowWidth, mWindowHeight), 0.0f, Color::White,
                               mBackGroundTexture, Vector4::UnitRect);
    }

    for (auto drawable: mDrawables)
        drawable->Draw(mRenderer);

    // Draw all UI screens
    mRenderer->BeginUIDraw();
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }
    //
    // if (mShowMap) {
    //     mMap->Draw(mRenderer);
    // }
    //

    if (mSceneManagerState == SceneManagerState::Entering ||
        mSceneManagerState == SceneManagerState::Exiting ||
        mIsCrossFading)
    {
        mRenderer->DrawFade(mFadeAlpha / 255.0f);
    }
    else if (mSceneManagerState == SceneManagerState::Active)
    {
        mRenderer->DrawFade(1.0f);
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

// SDL_Texture* Game::LoadTexture(const std::string &texturePath)
// {
//     SDL_Surface* surface = IMG_Load(texturePath.c_str());
//     if (!surface)
//     {
//         SDL_Log("Falha ao carregar imagem %s: %s", texturePath.c_str(), IMG_GetError());
//         return nullptr;
//     }
//
//     SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
//     SDL_FreeSurface(surface); // Libera a superfície, já não é mais necessária
//     surface = nullptr;
//
//     if (!texture)
//     {
//         SDL_Log("Falha ao criar textura a partir de %s: %s", texturePath.c_str(), SDL_GetError());
//         return nullptr;
//     }
//     return texture;
// }

UIFont* Game::LoadFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end()) {
        return iter->second;
    }

    // Fonte ainda não carregada, cria nova instância
    UIFont* font = new UIFont(mRenderer);
    if (font->Load(fileName)) {
        mFonts.emplace(fileName, font);
        return font;
    }
    else {
        // Falha no carregamento — limpa e retorna nullptr
        font->Unload();
        delete font;
        SDL_Log("Falha ao carregar fonte: %s", fileName.c_str());
        return nullptr;
    }
}

void Game::UnloadScene()
{
    if (mGamePlayState != GamePlayState::Cutscene) {
        mGamePlayState = GamePlayState::GameOver;
    }

    if (mPlayer) {
        mPlayer->SetState(ActorState::Paused);
    }

    for (auto it = mActors.begin(); it != mActors.end(); ) {
        Actor* actor = *it;
        if (actor != mPlayer) {
            it = mActors.erase(it);
            delete actor;
        } else {
            ++it;
        }
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();
    mHUD = nullptr;

    // Delete level data
    if (mLevelData != nullptr) {
        for (int i = 0; i < mLevelHeight; ++i) {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;
    mLevelData = nullptr;

    // Delete level data Dynamic Grounds
    if (mLevelDataDynamicGrounds != nullptr) {
        for (int i = 0; i < mLevelHeight; ++i) {
            if (mLevelDataDynamicGrounds[i] != nullptr)
                delete[] mLevelDataDynamicGrounds[i];
        }
    }
    delete[] mLevelDataDynamicGrounds;
    mLevelDataDynamicGrounds = nullptr;

    // SDL_DestroyTexture(mTileSheet);

    mTileSheetData.clear();

    mSpawnPoints.clear();

    // if (mBackGroundTexture) {
    //     SDL_DestroyTexture(mBackGroundTexture);
    //     mBackGroundTexture = nullptr;
    // }
    if (mBackGroundTexture) {
        // mBackGroundTexture->Unload();
        // delete mBackGroundTexture;
        mBackGroundTexture = nullptr;
    }

    mRenderer->UnloadAllTextures();
    mRenderer->ClearLights();

    delete mCamera;
    mCamera = nullptr;

    delete mWaveManager;
    mWaveManager = nullptr;
}

void Game::Shutdown()
{
    delete mPlayer;
    mPlayer = nullptr;

    delete mStore;
    mStore = nullptr;

    // Delete map
    // if (mMap) {
    //     delete mMap;
    //     mMap = nullptr;
    // }

    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }

    if (mController) {
        SDL_GameControllerClose(mController);
    }

    mFonts.clear();

    // Destroi textures
    if (mBackGroundTextureMainMenu) {
        SDL_DestroyTexture(mBackGroundTextureMainMenu);
        mBackGroundTextureMainMenu = nullptr;
    }
    if (mBackGroundTextureLevel1) {
        SDL_DestroyTexture(mBackGroundTextureLevel1);
        mBackGroundTextureLevel1 = nullptr;
    }
    // for (SDL_Texture*& t : mBackgroundLayersLevel2) {
    //     if (t) {
    //         SDL_DestroyTexture(t);
    //         t = nullptr;
    //     }
    // }
    mBackgroundLayersLevel2.clear();

    for (SDL_Texture*& t : mBackgroundLayersLevel3) {
        if (t) {
            SDL_DestroyTexture(t);
            t = nullptr;
        }
    }
    mBackgroundLayersLevel3.clear();

    for (SDL_Texture*& t : mBackgroundLayersLevel4) {
        if (t) {
            SDL_DestroyTexture(t);
            t = nullptr;
        }
    }
    mBackgroundLayersLevel4.clear();

    if (mBackGroundTextureLevel3) {
        SDL_DestroyTexture(mBackGroundTextureLevel3);
        mBackGroundTextureLevel3 = nullptr;
    }
    if (mBackGroundTextureLevel4) {
        SDL_DestroyTexture(mBackGroundTextureLevel4);
        mBackGroundTextureLevel4 = nullptr;
    }

    // Destroi audio
    delete mAudio;
    mAudio = nullptr;

    // Destroy Save Data
    delete mSaveData;
    delete mSaveManager;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::DrawParallaxBackground(Texture* background)
{
    float parallaxFactor = 0.55f; // fundo se move mais devagar que a câmera

    int bgWidth = background->GetWidth();
    int bgHeight = background->GetHeight();

    // Calcula o offset horizontal com base na câmera
    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % bgWidth;
    if (offsetX < 0) offsetX += bgWidth;

    // Desenha blocos horizontais suficientes para cobrir a largura da janela
    for (int x = -offsetX; x < mLogicalWindowWidth; x += bgWidth)
    {
        Vector2 position(x + bgWidth / 2, mLogicalWindowHeight / 2);
        Vector2 size(bgWidth, static_cast<int>(mLogicalWindowHeight));

        // Desenho via renderer — sem influência da câmera (passamos offset manualmente)
        mRenderer->DrawTexture(
            position,          // posição
            size,              // tamanho
            0.0f,              // sem rotação
            Vector3(1.0f, 1.0f, 1.0f), // cor normal
            background,        // textura
            Vector4::UnitRect,
            // GetCamera()->GetPosCamera(),
            Vector2(0.0f, 0.0f), // cameraPos = 0 para não aplicar deslocamento do Renderer
            Vector2(1.0f, 1.0f),
            1.0f               // textureFactor
        );
    }
}

void Game::DrawParallaxLayer(Texture* texture, float parallaxFactor, int y, int h)
{
    if (!texture) return;

    int texW = texture->GetWidth();
    int texH = texture->GetHeight();

    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % texW;
    if (offsetX < 0) offsetX += texW;

    // Cor branca (sem alteração)
    Vector3 color(1.0f, 1.0f, 1.0f);
    // Nenhum recorte de textura (usar textura inteira)
    Vector4 texRect(0.0f, 0.0f, 1.0f, 1.0f);
    // Nenhuma rotação
    float rotation = 0.0f;
    // Escala padrão (já está aplicado em texW/texH)
    Vector2 scale(1.0f, 1.0f);
    // Fator de textura 1.0 (mostrar textura normalmente)
    float textureFactor = 1.0f;

    for (int x = -offsetX; x < mLogicalWindowWidth; x += texW)
    {
        Vector2 pos(static_cast<float>(x + texW / 2), static_cast<float>(y + h / 2));
        Vector2 size(static_cast<float>(texW), static_cast<float>(h));

        mRenderer->DrawTexture(
            pos,                 // posição central do quad
            size,                // tamanho
            rotation,            // sem rotação
            color,               // cor
            texture,             // textura a desenhar
            texRect,             // retângulo completo da textura
            mCamera->GetPosCamera(), // posição da câmera para paralaxe
            scale,               // escala padrão
            textureFactor        // fator de textura
        );
    }
}

void Game::DrawParallaxLayers(std::vector<Texture*> backgroundLayers)
{
    const int numLayers = static_cast<int>(backgroundLayers.size());
    if (numLayers == 0) {
        return;
    }

    // Parallax fator mínimo e máximo (fundo mais lento, frente mais rápido)
    const float minFactor = 0.2f;  // fundo mais lento
    const float maxFactor = 1.0f;  // frente acompanha totalmente a câmera

    for (int i = 0; i < numLayers; ++i) {
        // Fator de parallaxe crescente do fundo (min) até frente (max)
        float t = static_cast<float>(i) / (numLayers - 1); // varia de 0 a 1
        float parallaxFactor = minFactor + t * (maxFactor - minFactor);

        Texture* texture = backgroundLayers[i];

        // Defina altura Y e altura H conforme necessário
        // Exemplo: tela inteira vertical
        int y = 0;
        int h = mLogicalWindowHeight;

        DrawParallaxLayer(texture, parallaxFactor, y, h);
    }
}

// void Game::ChangeResolution(float oldScale)
// {
//     mCheckpointPosition.x = mCheckpointPosition.x / oldScale * mScale;
//     mCheckpointPosition.y = mCheckpointPosition.y / oldScale * mScale;
//     mLavaRespawnPosition.x = mLavaRespawnPosition.x / oldScale * mScale;
//     mLavaRespawnPosition.y = mLavaRespawnPosition.y / oldScale * mScale;
//
//     for (auto& [key, value] : mSpawnPoints) {
//         value.x = value.x / oldScale * mScale;
//         value.y = value.y / oldScale * mScale;
//     }
//
//     mTileSize = mTileSize / oldScale * mScale;
//
//     for (auto actor : mActors) {
//         actor->ChangeResolution(oldScale, mScale);
//     }
//
//     if (mCamera) {
//         mCamera->ChangeResolution(oldScale, mScale);
//     }
//
//     for (auto UIScreen : mUIStack) {
//         UIScreen->ChangeResolution(oldScale, mScale);
//     }
//
//     if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
//         mLogicalWindowWidth = static_cast<float>(mWindowWidth);
//         mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
//         SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
//     }
//     else {
//         mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
//         mLogicalWindowHeight = static_cast<float>(mWindowHeight);
//         SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
//     }
//
//     if (mMap) {
//         mMap->ChangeResolution(oldScale, mScale);
//     }
// }
