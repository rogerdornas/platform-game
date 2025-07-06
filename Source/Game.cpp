#include <algorithm>
#include <vector>
#include "Game.h"
// #include "Components/DrawComponent.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/ParticleSystem.h"
#include <iostream>
#include <fstream>
#include <map>
#include "CSV.h"
#include "Json.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "HUD.h"
#include "Actors/BushMonster.h"
#include "Actors/Checkpoint.h"
#include "UIElements/UIScreen.h"
#include "Actors/DynamicGround.h"
#include "Actors/Fox.h"
#include "Actors/Frog.h"
#include "Actors/Lever.h"
#include "Actors/Trigger.h"
#include "Actors/Fairy.h"
#include "Actors/FlyingShooterEnemy.h"
#include "Actors/Golem.h"
#include "Actors/Mantis.h"
#include "Actors/Money.h"
#include "Actors/Moth.h"
#include "Actors/Projectile.h"
#include "Components/AABBComponent.h"
#include "Components/DrawComponents/DrawAnimatedComponent.h"


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
    :mResetLevel(false)
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
    ,mCamera(nullptr)
    ,mPlayer(nullptr)
    ,mLevelData(nullptr)
    ,mLevelDataDynamicGrounds(nullptr)
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
    ,mPlayerDeathCounter(0)
    ,mCheckpointPosition(Vector2::Zero)
    ,mCheckPointMoney(0)
    ,mGoingToNextLevel(false)
    ,mIsPlayingOnKeyboard(true)
    ,mLeftStickYState(StickState::Neutral)
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
    ,mPauseMenu(nullptr)
    ,mSceneManagerState(SceneManagerState::None)
    ,mFadeDuration(0.5f)
    ,mSceneManagerTimer(0.0f)
    ,mFadeAlpha(0)
    ,mGameScene(GameScene::MainMenu)
    ,mNextScene(GameScene::MainMenu)
    ,mContinueScene(GameScene::Prologue)
{
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Echoes of Elementum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               mWindowWidth, mWindowHeight,
                               // SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                               SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE);

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

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

    if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
        mLogicalWindowWidth = static_cast<float>(mWindowWidth);
        mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
        float ratio = mOriginalWindowWidth / static_cast<float>(mLogicalWindowWidth);
        int tileSize = static_cast<int>(mOriginalTileSize / ratio);
        mScale = static_cast<float>(tileSize) / mOriginalTileSize;
    }
    else {
        mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
        mLogicalWindowHeight = static_cast<float>(mWindowHeight);
        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
        float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
        int tileSize = static_cast<int>(mOriginalTileSize / ratio);
        mScale = static_cast<float>(tileSize) / mOriginalTileSize;
    }


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

    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/7.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/6.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/5.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/4.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/3.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/2.png"));
    mBackgroundLayersLevel2.emplace_back(LoadTexture(backgroundAssets + "Level2/1.png"));

    mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/1.png"));
    mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/2.png"));
    // mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/3.png"));
    mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/4.png"));
    mBackgroundLayersLevel3.emplace_back(LoadTexture(backgroundAssets + "Level3/5.png"));

    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/1.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/2.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/3.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/4.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/5.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/6.png"));
    mBackgroundLayersLevel4.emplace_back(LoadTexture(backgroundAssets + "Level4/7.png"));

    mTicksCount = SDL_GetTicks();

    SetGameScene(GameScene::MainMenu);

    mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");

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
            scene == GameScene::Prologue ||
            scene == GameScene::Level1 ||
            scene == GameScene::Level2 ||
            scene == GameScene::Level3 ||
            scene == GameScene::Level4 ||
            scene == GameScene::Level5) {
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
        mAudio->StopAllSounds();
    }

    const std::string backgroundAssets = "../Assets/Sprites/Background/";
    const std::string levelsAssets = "../Assets/Levels/";

    if (mNextScene != GameScene::MainMenu) {
        // Pool de Fireballs
        for (int i = 0; i < 10; i++) {
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
            auto* fairy = new Fairy(this, 40, 40);
        }

        // Volta player
        if (mPlayer) {
            mPlayer->GetComponent<DrawAnimatedComponent>()->SetIsVisible(true);
        }

        // Guarda último level que o player estava
        mContinueScene = mNextScene;
        mIsPaused = false;
    }
    else {
        // Se está no menu, pausa draw de player
        if (mPlayer) {
            mPlayer->GetComponent<DrawAnimatedComponent>()->SetIsVisible(false);
        }
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
        mBackGroundTexture = LoadTexture(backgroundAssets + "Menu6.png");

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

    else if (mNextScene == GameScene::Prologue) {
        mUseParallaxBackground = false;
        // mUseParallaxBackground = true;
        mBackGroundTexture = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
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

    // Verifica as 2 primeiras mortes do player para tocar cutscene
    if (mPlayerDeathCounter < 3 && mPlayer) {
        if (mPlayerDeathCounter < mPlayer->GetDeathCounter() && mGamePlayState == GamePlayState::Playing) {
            mPlayerDeathCounter = mPlayer->GetDeathCounter();
            if (mPlayerDeathCounter == 1) {
                mCurrentCutscene = new Cutscene(this, "primeiraMortePlayer", "../Assets/Cutscenes/Cutscenes.json");
                mCurrentCutscene->Start();
                SetCurrentCutscene(mCurrentCutscene);
                SetGamePlayState(Game::GamePlayState::Cutscene);
            }
            if (mPlayerDeathCounter == 2) {
                mCurrentCutscene = new Cutscene(this, "segundaMortePlayer", "../Assets/Cutscenes/Cutscenes.json");
                mCurrentCutscene->Start();
                SetCurrentCutscene(mCurrentCutscene);
                SetGamePlayState(Game::GamePlayState::Cutscene);
            }
        }
    }

    // Set new scene
    mGameScene = mNextScene;
}

void Game::LoadMainMenu() {
    auto mainMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(mLogicalWindowWidth / 5, 50 * mScale);
    mainMenu->SetSize(Vector2(mLogicalWindowWidth / 3, mLogicalWindowHeight / 3));
    mainMenu->SetPosition(Vector2(mLogicalWindowWidth / 3, 2 * mLogicalWindowHeight / 3));
    Vector2 buttonPos = Vector2((mainMenu->GetSize().x - buttonSize.x) / 2, 0);

    std::string name = "CONTINUAR";
    int buttonPointSize = static_cast<int>(34 * mScale);
    mainMenu->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() { SetGameScene(mContinueScene, 0.5f); });

    name = "NOVO JOGO";
    mainMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        SetGameScene(GameScene::Prologue, 0.5f);
        delete mPlayer;
        mPlayer = nullptr;
        mPlayerDeathCounter = 0;
        delete mStore;
        mStore = nullptr;
        mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
    });

    name = "SELECIONAR FASE";
    mainMenu->AddButton(name, buttonPos + Vector2(0, 4 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() { LoadLevelSelectMenu(); });

    name = "OPÇÕES";
    mainMenu->AddButton(name, buttonPos + Vector2(0, 6 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() { LoadOptionsMenu(); });

    name = "SAIR";
    mainMenu->AddButton(name, buttonPos + Vector2(0, 8 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() { Quit(); });
}

UIScreen* Game::LoadPauseMenu() {
    mPauseMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    const Vector2 buttonSize = Vector2(mLogicalWindowWidth * 0.22f, 50 * mScale);
    mPauseMenu->SetSize(Vector2(mLogicalWindowWidth / 3, mLogicalWindowHeight / 3));
    mPauseMenu->SetPosition(Vector2(mLogicalWindowWidth / 3, 5 * mLogicalWindowHeight / 12));
    Vector2 buttonPos = Vector2((mPauseMenu->GetSize().x - buttonSize.x) / 2, 0);

    std::string name = "CONTINUAR";
    int buttonPointSize = static_cast<int>(34 * mScale);
    mPauseMenu->AddButton(name, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        TogglePause();
        mPauseMenu->Close();
    });

    name = "OPÇÕES";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        LoadOptionsMenu();
    });

    name = "VOLTAR AO CHECKPOINT";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 4 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        ResetGameScene(0.2f);
        mPlayer->ResetHealthPoints();
        mPlayer->ResetMana();
        mPlayer->ResetHealCount();
        mPlayer->SetMoney(mCheckPointMoney);

        mPauseMenu->Close();
        if (mStore->StoreOpened()) {
            mStore->CloseStore();
        }
        if (mStore->StoreMessageOpened()) {
            mStore->CloseStoreMessage();
        }
    });

    name = "VOLTAR AO MENU";
    mPauseMenu->AddButton(name, buttonPos + Vector2(0, 6 * 35) * mScale, buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        SetGameScene(GameScene::MainMenu, 0.5f);
        mPauseMenu->Close();
        if (mStore->StoreOpened()) {
            mStore->CloseStore();
        }
    });

    return mPauseMenu;
}

void Game::LoadLevelSelectMenu() {
    mLevelSelectMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mLevelSelectMenu->SetSize(Vector2(mLogicalWindowWidth * 0.8f, mLogicalWindowHeight * 0.85f));
    mLevelSelectMenu->SetPosition(Vector2(mLogicalWindowWidth * 0.1f, mLogicalWindowHeight * 0.13f));

    const auto buttonSize = Vector2(mLevelSelectMenu->GetSize().x * 0.8f, 50 * mScale);
    const auto buttonPointSize = static_cast<int>(34 * mScale);
    const auto buttonPos = Vector2(mLevelSelectMenu->GetSize().x * 0.1f, 0);

    mLevelSelectMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", Vector2::Zero, mLevelSelectMenu->GetSize());

    std::string name = "   PRÓLOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Prologue, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "   1 - FLORESTA";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 4 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Level1, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "   2 - FOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 6 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Level2, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "   3 - PÂNTANO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 8 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Level3, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "   4 - NEVE";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 10 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Level4, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "   5 - FINAL";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 12 * 35) * mScale,
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            SetGameScene(GameScene::Level5, 0.5f);
            delete mPlayer;
            mPlayer = nullptr;
            mPlayerDeathCounter = 0;
            delete mStore;
            mStore = nullptr;
            mStore = new Store(this, "../Assets/Fonts/K2D-Bold.ttf");
        });

    name = "VOLTAR";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, mLevelSelectMenu->GetSize().y - buttonSize.y * 1.2f),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() { mLevelSelectMenu->Close(); });
}

void Game::LoadOptionsMenu() {
    mOptionsMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mOptionsMenu->SetSize(Vector2(mLogicalWindowWidth * 0.8f, mLogicalWindowHeight * 0.85f));
    mOptionsMenu->SetPosition(Vector2(mLogicalWindowWidth * 0.1f, mLogicalWindowHeight * 0.13f));
    Vector2 buttonSize = Vector2(mOptionsMenu->GetSize().x * 0.8f, 50 * mScale);
    Vector2 buttonPos = Vector2(mOptionsMenu->GetSize().x * 0.1f, 0);

    mOptionsMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", Vector2::Zero, mOptionsMenu->GetSize());

    UIText* text;
    std::string name;
    int buttonPointSize = static_cast<int>(34 * mScale);
    Vector2 textPos = Vector2(buttonSize.x * 0.05f, 0);
    std::string optionValue;
    float optionPosX = mOptionsMenu->GetSize().x * 0.6f;
    UIButton* button;

    if (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        name = "FULL SCREEN";
        button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y * 1.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]() {
            SDL_SetWindowFullscreen(mWindow, 0);
            mOptionsMenu->Close();
            LoadOptionsMenu();
        }, textPos);
        optionValue = "< ON >";
        text = mOptionsMenu->AddText(optionValue, Vector2::Zero, Vector2::Zero, buttonPointSize);
        text->SetPosition(Vector2(optionPosX, button->GetPosition().y));
    }
    else {
        name = "FULL SCREEN";
        button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y * 1.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]() {
            SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            mOptionsMenu->Close();
            LoadOptionsMenu();
        }, textPos);
        optionValue = "< OFF >";
        text = mOptionsMenu->AddText(optionValue, Vector2::Zero, Vector2::Zero, buttonPointSize);
        text->SetPosition(Vector2(optionPosX, button->GetPosition().y));
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
    button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y * 3.0f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadKeyBoardMenu();
    }, textPos);

    name = "CONTROLE";
    button = mOptionsMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y * 4.5f), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadControlMenu();
    }, textPos);

    name = "VOLTAR";
    mOptionsMenu->AddButton(name, buttonPos + Vector2(0, mOptionsMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mOptionsMenu->Close();
    });
}

void Game::LoadControlMenu() {
    mControlMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mControlMenu->SetSize(Vector2(mLogicalWindowWidth * 0.8f, mLogicalWindowHeight * 0.85f));
    mControlMenu->SetPosition(Vector2(mLogicalWindowWidth * 0.1f, mLogicalWindowHeight * 0.13f));
    Vector2 buttonSize = Vector2(mControlMenu->GetSize().x * 0.8f, 50 * mScale);
    Vector2 buttonPos = Vector2(mControlMenu->GetSize().x * 0.1f, 0);

    mControlMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", Vector2::Zero, mControlMenu->GetSize());
    mControlMenu->AddImage("../Assets/Sprites/Menus/Control2.png", Vector2::Zero, Vector2(mControlMenu->GetSize().x, mControlMenu->GetSize().x / 1.9f));

    std::string name;
    int buttonPointSize = static_cast<int>(34 * mScale);

    name = "VOLTAR";
    mControlMenu->AddButton(name, buttonPos + Vector2(0, mControlMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mControlMenu->Close();
    });
}

void Game::LoadKeyBoardMenu() {
    mKeyboardMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mKeyboardMenu->SetSize(Vector2(mLogicalWindowWidth * 0.8f, mLogicalWindowHeight * 0.85f));
    mKeyboardMenu->SetPosition(Vector2(mLogicalWindowWidth * 0.1f, mLogicalWindowHeight * 0.13f));
    Vector2 buttonSize = Vector2(mKeyboardMenu->GetSize().x * 0.8f, 50 * mScale);
    Vector2 buttonPos = Vector2(mKeyboardMenu->GetSize().x * 0.1f, 0);

    mKeyboardMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", Vector2::Zero, mKeyboardMenu->GetSize());
    mKeyboardMenu->AddImage("../Assets/Sprites/Menus/Keyboard4.png", Vector2(mKeyboardMenu->GetSize().x * 0.125f, 0), Vector2(mKeyboardMenu->GetSize().y * 1.4f, mKeyboardMenu->GetSize().y * 1.4f / 1.52f));

    std::string name;
    int buttonPointSize = static_cast<int>(34 * mScale);


    name = "VOLTAR";
    mKeyboardMenu->AddButton(name, buttonPos + Vector2(0, mKeyboardMenu->GetSize().y - buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        mKeyboardMenu->Close();
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
    Ground* ground;
    for (const auto &layer: mapData["layers"]) {
        if (layer["name"] == "Grounds") {
            for (const auto &obj: layer["objects"]) {
                std::string name = obj["name"];
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
                            minHeight = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "MinWidth") {
                            minWidth = static_cast<float>(prop["value"]) * mScale;
                        }
                        else if (propName == "Oscillate") {
                            isOscillating = static_cast<float>(prop["value"]);
                        }
                    }
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
                    dynamicGround->SetSprites();
                }
                else {
                    ground = new Ground(this, width, height, isSpike, isMoving, movingDuration, Vector2(speedX, speedY));
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetSprites();
                }
            }
        }
        if (layer["name"] == "Triggers") {
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
                std::string scene;
                std::string dialoguePath;
                std::string cutsceneId;
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
                        else if (propName == "Scene") {
                            scene = prop["value"];
                        }
                        else if (propName == "FilePath") {
                            dialoguePath = prop["value"];
                        }
                        else if (propName == "CutsceneId") {
                            cutsceneId = prop["value"];
                        }
                    }
                }
                groundsIds = ParseIntList(grounds);
                enemiesIds = ParseIntList(enemies);

                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetGroundsIds(groundsIds);
                trigger->SetEnemiesIds(enemiesIds);
                trigger->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                trigger->SetScene(scene);
                trigger->SetDialoguePath(dialoguePath);
                trigger->SetCutsceneId(cutsceneId);
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
                if (name == "Enemy Simple") {
                    auto* enemySimple = new EnemySimple(this, 53, 45, 200, 30);
                    enemySimple->SetPosition(Vector2(x, y));
                    enemySimple->SetId(id);
                }
                else if (name == "Flying Enemy") {
                    auto* flyingEnemySimple = new FlyingEnemySimple(this, 70, 70, 250, 60);
                    flyingEnemySimple->SetPosition(Vector2(x, y));
                    flyingEnemySimple->SetId(id);
                }
                else if (name == "FlyingShooterEnemy") {
                    auto* flyingShooterEnemy = new FlyingShooterEnemy(this, 70, 70, 250, 60);
                    flyingShooterEnemy->SetPosition(Vector2(x, y));
                    flyingShooterEnemy->SetId(id);
                }
                else if (name == "Mantis") {
                    auto* mantis = new Mantis(this, 120, 120, 250, 90);
                    mantis->SetPosition(Vector2(x, y));
                    mantis->SetId(id);
                }
                else if (name == "Fox") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "UnlockGrounds") {
                                grounds = prop["value"];
                            }
                        }
                    }
                    ids = ParseIntList(grounds);
                    auto* fox = new Fox(this, 100, 170, 300, 700);
                    fox->SetPosition(Vector2(x, y));
                    fox->SetId(id);
                    fox->SetUnlockGroundsIds(ids);
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
                        }
                    }
                    ids = ParseIntList(grounds);
                    auto* frog = new Frog(this, 165, 165, 300, 500);
                    frog->SetPosition(Vector2(x, y));
                    frog->SetId(id);
                    frog->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    frog->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                    frog->SetUnlockGroundsIds(ids);
                }
                else if (name == "Moth") {
                    auto* moth = new Moth(this, 200, 200, 500, 1000);
                    moth->SetPosition(Vector2(x, y));
                    moth->SetId(id);
                }
                else if (name == "BushMonster") {
                    auto* bushMonster = new BushMonster(this, 220, 140, 300, 300);
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
                                MinPosY =static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]) * mScale;
                            }
                            else if (propName == "UnlockGrounds") {
                                grounds = prop["value"];
                            }
                        }
                    }
                    auto* golem = new Golem(this, 144, 190, 600, 400);
                    golem->SetPosition(Vector2(x, y));
                    golem->SetId(id);
                    golem->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    golem->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
            }
        }
        if (layer["name"] == "Checkpoint") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;

                auto checkpoint = new Checkpoint(this, width, height, Vector2(x + width / 2, y + height / 2));
            }
        }

        if (layer["name"] == "Player") {
            for (const auto &obj: layer["objects"]) {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                if (mPlayer) {
                    mPlayer->SetSword();
                    mPlayer->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2::Zero);
                    mPlayer->SetIsDead(false);

                    // Faz isso para o player ser sempre o último a ser atualizado a cada frame
                    RemoveActor(mPlayer);
                    AddActor(mPlayer);
                }
                else {
                    mPlayer = new Player(this, 60, 85);
                    mPlayer->SetPosition(Vector2(x, y));
                    mPlayer->SetStartingPosition(Vector2(x, y));
                    mCheckpointPosition = Vector2(x, y);
                    mCheckPointMoney = mPlayer->GetMoney();
                }

                if (mGoingToNextLevel) {
                    mCheckpointPosition = Vector2(x, y);
                    mCheckPointMoney = mPlayer->GetMoney();
                    mGoingToNextLevel = false;
                }

                mPlayer->SetState(ActorState::Active);
                mPlayer->SetPosition(mCheckpointPosition);
                mPlayer->GetComponent<AABBComponent>()->SetActive(true);
            }
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
    int tileSize = int(mapData["tilewidth"]) * mScale;
    mLevelHeight = height;
    mLevelWidth = width;
    mTileSize = tileSize;

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
        }
    }

    // Load tilesheet texture
    size_t pos = fileName.rfind(".json");
    std::string tileSheetTexturePath = fileName.substr(0, pos) + ".png";
    mTileSheet = LoadTexture(tileSheetTexturePath);

    // Load tilesheet data
    std::string tileSheetDataPath = fileName.substr(0, pos) + "TileSet.json";
    std::ifstream tileSheetFile(tileSheetDataPath);

    nlohmann::json tileSheetData = nlohmann::json::parse(tileSheetFile);

    for (const auto &tile: tileSheetData["sprites"]) {
        std::string tileFileName = tile["fileName"];
        int x = tile["x"].get<int>();
        int y = tile["y"].get<int>();
        int w = tile["width"].get<int>();
        int h = tile["height"].get<int>();

        size_t dotPos = tileFileName.find('.');
        std::string numberStr = tileFileName.substr(0, dotPos);
        int index = std::stoi(numberStr); // converte para inteiro

        mTileSheetData[index] = SDL_Rect{x, y, w, h};
    }

    // Cria objetos
    LoadObjects(fileName);
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
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    float oldScale = mScale;
                    mWindowWidth = event.window.data1;
                    mWindowHeight = event.window.data2;
                    if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
                        // Comenta essa parte para tirar o zoom do mapa
                        mLogicalWindowWidth = static_cast<float>(mWindowWidth);
                        mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
                        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);

                        float ratio = mOriginalWindowWidth / static_cast<float>(mLogicalWindowWidth);
                        int tileSize = static_cast<int>(mOriginalTileSize / ratio);
                        mScale = static_cast<float>(tileSize) / mOriginalTileSize;
                    }
                    else {
                        // Comenta essa parte para tirar o zoom do mapa
                        mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
                        mLogicalWindowHeight = static_cast<float>(mWindowHeight);
                        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);

                        float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
                        int tileSize = static_cast<int>(mOriginalTileSize / ratio);
                        mScale = static_cast<float>(tileSize) / mOriginalTileSize;
                    }
                    // const float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
                    // const int tileSize = static_cast<int>(32 / ratio);
                    // mScale = static_cast<float>(tileSize) / 32.0f;
                    ChangeResolution(oldScale);
                }
                break;

            case SDL_KEYDOWN:
                if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = true;
                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym, SDL_CONTROLLER_BUTTON_INVALID, 0);
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
                        if (!mStore->StoreOpened() && mGameScene != GameScene::MainMenu &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                if (mUIStack.back() == mPauseMenu) {
                                    mPauseMenu->Close();
                                    TogglePause();
                                }
                            }
                            else {
                                TogglePause();
                                LoadPauseMenu();
                            }
                        }
                    }

                    // if (event.key.keysym.sym == SDLK_8)
                    //     Quit();
                    //
                    // if (event.key.keysym.sym == SDLK_5) {
                    //     mIsSlowMotion = !mIsSlowMotion;
                    //     mIsAccelerated = false;
                    // }
                    //
                    // if (event.key.keysym.sym == SDLK_6) {
                    //     mIsAccelerated = !mIsAccelerated;
                    //     mIsSlowMotion = false;
                    // }
                }
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = false;

                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, event.cbutton.button, 0);
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
                        if (!mStore->StoreOpened() && mGameScene != GameScene::MainMenu &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                if (mUIStack.back() == mPauseMenu) {
                                    mPauseMenu->Close();
                                    TogglePause();
                                }
                            }
                            else {
                                TogglePause();
                                LoadPauseMenu();
                            }
                        }
                    }
                }
                break;

            case SDL_CONTROLLERAXISMOTION:
                if (mGamePlayState != GamePlayState::GameOver) {
                    if (Math::Abs(event.caxis.value) > DEAD_ZONE) {
                        mIsPlayingOnKeyboard = false;
                    }

                    if (!mUIStack.empty()) {
                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                            int value = event.caxis.value;

                            if (value < -DEAD_ZONE) {
                                if (mLeftStickYState != StickState::Up) {
                                    mLeftStickYState = StickState::Up;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, value);
                                    }
                                }
                            }
                            else if (value > DEAD_ZONE) {
                                if (mLeftStickYState != StickState::Down) {
                                    mLeftStickYState = StickState::Down;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(SDLK_UNKNOWN, SDL_CONTROLLER_BUTTON_INVALID, value);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                mLeftStickYState = StickState::Neutral;
                            }
                        }
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (mGamePlayState != GamePlayState::GameOver) {
                    mIsPlayingOnKeyboard = true;

                    // Handle mouse for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleMouse(event);
                    }
                    break;

                    case SDL_MOUSEMOTION:
                        mIsPlayingOnKeyboard = true;

                    // Handle mouse for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleMouse(event);
                    }
                }
                break;

            default:
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mGamePlayState == GamePlayState::Playing) {
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

    // testes para alterar velocidade do jogo
    if (mIsSlowMotion) {
        deltaTime *= 0.5;
    }
    if (mIsAccelerated) {
        deltaTime *= 1.5;
    }

    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255); // Usado para deixar as bordas em preto
    SDL_RenderClear(mRenderer);

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

    if (mResetLevel) {
        mStore->CloseStoreMessage();
        ResetGameScene(1.5f);
        mPlayer->ResetHealthPoints();
        mPlayer->ResetMana();
        mPlayer->ResetHealCount();
        mPlayer->SetMoney(mCheckPointMoney);
        mResetLevel = false;
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

void Game::RemoveGround(class Ground* g)
{
    auto iter = std::find(mGrounds.begin(), mGrounds.end(), g);
    if (iter != mGrounds.end())
        mGrounds.erase(iter);
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

void Game::RemoveFireball(class FireBall* f)
{
    auto iter = std::find(mFireBalls.begin(), mFireBalls.end(), f);
    if (iter != mFireBalls.end())
        mFireBalls.erase(iter);
}

void Game::AddParticle(class Particle* p) { mParticles.emplace_back(p); }

void Game::RemoveParticle(class Particle* p)
{
    auto iter = std::find(mParticles.begin(), mParticles.end(), p);
    if (iter != mParticles.end())
        mParticles.erase(iter);
}

void Game::AddProjectile(class Projectile* p) { mProjectiles.emplace_back(p); }

void Game::RemoveProjectile(class Projectile* p)
{
    auto iter = std::find(mProjectiles.begin(), mProjectiles.end(), p);
    if (iter != mProjectiles.end())
        mProjectiles.erase(iter);
}

void Game::AddMoney(class Money* m) { mMoneys.emplace_back(m); }

void Game::RemoveMoney(class Money* m)
{
    auto iter = std::find(mMoneys.begin(), mMoneys.end(), m);
    if (iter != mMoneys.end())
        mMoneys.erase(iter);
}

void Game::AddEnemy(class Enemy* e) { mEnemies.emplace_back(e); }

void Game::RemoveEnemy(class Enemy* e)
{
    auto iter = std::find(mEnemies.begin(), mEnemies.end(), e);
    if (iter != mEnemies.end())
        mEnemies.erase(iter);
}

Enemy* Game::GetEnemyById(int id) {
    for (Enemy* e : mEnemies) {
        if (e->GetId() == id) {
            return e;
        }
    }
    return nullptr;
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
        mPendingActors.emplace_back(actor);

    else
        mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent* drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(), [](const DrawComponent* a, const DrawComponent* b)
    {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent* drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
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
            mContinueScene = GameScene::Prologue;
            mGoingToNextLevel = true;
            mIsPlayingFinalCutscene = false;
        }
    }
}

void Game::PlayFinalEvilCutscene() {

}


void Game::GenerateOutput()
{
    // Clear back buffer
    SDL_RenderClear(mRenderer);

    if (mCamera) {
        if (mUseParallaxBackground) {
            switch (mGameScene) {
                case GameScene::LevelTeste:
                    DrawParallaxLayers(mBackgroundLayersLevel2);
                break;

                case GameScene::Level1:
                    DrawParallaxLayers(mBackgroundLayersLevel3);
                    break;

                case GameScene::Level2:
                    DrawParallaxLayers(mBackgroundLayersLevel2);
                    break;

                case GameScene::Level3:
                    DrawParallaxLayers(mBackgroundLayersLevel3);
                    break;

                case GameScene::Level4:
                    DrawParallaxLayers(mBackgroundLayersLevel4);
                    break;

                case GameScene::Level5:
                    DrawParallaxLayers(mBackgroundLayersLevel4);
                    break;

                default:
                    break;
            }
        }
        else {
            DrawParallaxBackground(mBackGroundTexture); // desenha o fundo com repetição horizontal
        }
        // Ordem de desenho: mais distantes primeiro
        // DrawParallaxLayer(mSky,        0.1f, 0, mWindowHeight / 2);  // camada mais distante
        // DrawParallaxLayer(mMountains,  0.3f, mWindowHeight / 4, mWindowHeight / 3);  // montanhas ao fundo
        // DrawParallaxLayer(mTreesBack,  0.5f, mWindowHeight / 3, mWindowHeight / 2);  // árvores distantes
        // DrawParallaxLayer(mTreesFront, 0.7f, mWindowHeight / 2, mWindowHeight / 2);  // árvores próximas
    }
    else {
        SDL_Rect dest = {
            0,
            0,
            static_cast<int>(mLogicalWindowWidth),
            static_cast<int>(mLogicalWindowHeight)
        };
        SDL_RenderCopy(mRenderer, mBackGroundTexture, nullptr, &dest);
    }

    for (auto drawable: mDrawables)
        drawable->Draw(mRenderer);

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    if (mSceneManagerState == SceneManagerState::Entering || mSceneManagerState == SceneManagerState::Exiting) {
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        // Define a cor preta (RGBA)
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, mFadeAlpha);

        // Cria o retângulo cobrindo toda a tela
        SDL_Rect fullScreenRect = { 0, 0, mWindowWidth, mWindowHeight };

        // Desenha o retângulo preenchido
        SDL_RenderFillRect(mRenderer, &fullScreenRect);
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        // Define a cor preta (RGBA)
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

        // Cria o retângulo cobrindo toda a tela
        SDL_Rect fullScreenRect = { 0, 0, mWindowWidth, mWindowHeight };

        // Desenha o retângulo preenchido
        SDL_RenderFillRect(mRenderer, &fullScreenRect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string &texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface)
    {
        SDL_Log("Falha ao carregar imagem %s: %s", texturePath.c_str(), IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface); // Libera a superfície, já não é mais necessária

    if (!texture)
    {
        SDL_Log("Falha ao criar textura a partir de %s: %s", texturePath.c_str(), SDL_GetError());
        return nullptr;
    }
    return texture;
}

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

    SDL_DestroyTexture(mTileSheet);
    mTileSheet = nullptr;

    if (mBackGroundTexture) {
        SDL_DestroyTexture(mBackGroundTexture);
        mBackGroundTexture = nullptr;
    }

    delete mCamera;
    mCamera = nullptr;
}

void Game::Shutdown()
{
    delete mPlayer;
    mPlayer = nullptr;
    delete mStore;
    mStore = nullptr;
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
    for (SDL_Texture*& t : mBackgroundLayersLevel2) {
        if (t) {
            SDL_DestroyTexture(t);
            t = nullptr;
        }
    }
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

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::DrawParallaxBackground(SDL_Texture* background)
{
    float parallaxFactor = 0.55f; // fundo se move mais devagar que a câmera

    int bgWidth, bgHeight;
    SDL_QueryTexture(background, nullptr, nullptr, &bgWidth, &bgHeight);
    bgWidth *= mScale;
    bgHeight *= mScale;
    // Calcula o offset horizontal com base na câmera
    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % bgWidth;
    if (offsetX < 0) offsetX += bgWidth;

    // Desenha blocos horizontais suficientes para cobrir a largura da janela
    for (int x = -offsetX; x < mLogicalWindowWidth; x += bgWidth)
    {
        SDL_Rect dest = {
            x,
            0,
            bgWidth,
            static_cast<int>(mLogicalWindowHeight)
        };

        SDL_RenderCopy(mRenderer, background, nullptr, &dest);
    }
}

void Game::DrawParallaxLayer(SDL_Texture* texture, float parallaxFactor, int y, int h)
{
    int texW, texH;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
    texW *= mScale;
    texH *= mScale;

    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % texW;
    if (offsetX < 0) offsetX += texW;

    for (int x = -offsetX; x < mLogicalWindowWidth; x += texW)
    {
        SDL_Rect dest = {
            x,
            y,
            texW,
            h
            // static_cast<int>(mLogicalWindowHeight)
        };

        SDL_RenderCopy(mRenderer, texture, nullptr, &dest);
    }
}

void Game::DrawParallaxLayers(std::vector<SDL_Texture*> backgroundLayers)
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

        SDL_Texture* texture = backgroundLayers[i];

        // Defina altura Y e altura H conforme necessário
        // Exemplo: tela inteira vertical
        int y = 0;
        int h = mLogicalWindowHeight;

        DrawParallaxLayer(texture, parallaxFactor, y, h);
    }
}

void Game::ChangeResolution(float oldScale)
{
    mCheckpointPosition.x = mCheckpointPosition.x / oldScale * mScale;
    mCheckpointPosition.y = mCheckpointPosition.y / oldScale * mScale;

    mTileSize = mOriginalTileSize * mScale;
    for (auto actor : mActors) {
        actor->ChangeResolution(oldScale, mScale);
    }

    if (mCamera) {
        mCamera->ChangeResolution(oldScale, mScale);
    }

    for (auto UIScreen : mUIStack) {
        UIScreen->ChangeResolution(oldScale, mScale);
    }

    if (static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight) < mOriginalWindowWidth / mOriginalWindowHeight) {
        mLogicalWindowWidth = static_cast<float>(mWindowWidth);
        mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
    }
    else {
        mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
        mLogicalWindowHeight = static_cast<float>(mWindowHeight);
        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
    }
}
