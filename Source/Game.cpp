// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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

#include "Actors/DynamicGround.h"
#include "Actors/Fox.h"
#include "Actors/Trigger.h"

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
    : mResetLevel(false),
      mWindow(nullptr),
      mRenderer(nullptr),
      mWindowWidth(windowWidth),
      mWindowHeight(windowHeight),
      mLogicalWindowWidth(windowWidth),
      mLogicalWindowHeight(windowHeight),
      mTicksCount(0),
      mIsRunning(true),
      mUpdatingActors(false),
      mFPS(FPS),
      mIsPaused(false),
      mCamera(nullptr),
      mPlayer(nullptr),
      mController(nullptr),
      mHitstopActive(false),
      mHitstopDuration(0.15f),
      mHitstopTimer(0.0f),
      mBackGroundTexture(nullptr),
      mSky(nullptr),
      mMountains(nullptr),
      mTreesBack(nullptr),
      mTreesFront(nullptr)
{
    float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
    int tileSize = static_cast<int>(mOriginalTileSize / ratio);
    mScale = static_cast<float>(tileSize) / mOriginalTileSize;
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Game-v0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               mWindowWidth, mWindowHeight,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                               // SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
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

    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1. (1 linha): Uma biblioteca Random.h foi incluída nesse projeto para a geração de números aleatórios.
    //  Utilize a função Random::Init para inicializar o gerador de números aleatórios (~1 linha).
    Random::Init();

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    InitializeActors();

    const std::string backgroundAssets = "../Assets/Sprites/Background/";
    // mBackGroundTexture = LoadTexture(backgroundAssets + "fundoCortadoEspichado.png");
    mBackGroundTexture = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art5.png");
    // mBackGroundTexture = LoadTexture(backgroundAssets + "Free-Nature-Backgrounds-Pixel-Art4.png");
    mSky = LoadTexture(backgroundAssets + "sky_cloud.png");
    mMountains = LoadTexture(backgroundAssets + "mountain2.png");
    mTreesBack = LoadTexture(backgroundAssets + "pine1.png");
    mTreesFront = LoadTexture(backgroundAssets + "pine2.png");

    return true;
}

void Game::InitializeActors()
{
    // Pool de Fireballs
    for (int i = 0; i < 5; i++)
        new FireBall(this);

    const std::string levelsAssets = "../Assets/Levels/";

    LoadMapMetadata(levelsAssets + "Forest/Forest.json");
    // LoadMapMetadata(levelsAssets + "Pain/Pain.json");
    // LoadMapMetadata(levelsAssets + "Run/Run.json");

    mLevelData = LoadLevel(levelsAssets + "Forest/Forest.csv", mLevelWidth, mLevelHeight);
    // mLevelData = LoadLevel(levelsAssets + "Pain/Pain.csv", mLevelWidth, mLevelHeight);
    // mLevelData = LoadLevel(levelsAssets + "Run/Run.csv", mLevelWidth, mLevelHeight);
    if (!mLevelData)
        return;

    LoadObjects(levelsAssets + "Forest/Forest.json");
    // LoadObjects(levelsAssets + "Pain/Pain.json");
    // LoadObjects(levelsAssets + "Run/Run.json");

    mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                       mPlayer->GetPosition().y - mLogicalWindowHeight / 2));
}

void Game::LoadMapMetadata(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Erro ao abrir o arquivo");
        return;
    }

    nlohmann::json mapData;
    file >> mapData;
    int height = int(mapData["height"]);
    int width = int(mapData["width"]);
    int tileSize = int(mapData["tilewidth"]) * mScale;
    mLevelHeight = height;
    mLevelWidth = width;
    mTileSize = tileSize;
}

int **Game::LoadLevel(const std::string &fileName, int width, int height)
{
    // TODO 5: Implemente essa função para carregar o nível a partir do arquivo CSV. Ela deve retornar um
    //  ponteiro para uma matriz 2D de inteiros. Cada linha do arquivo CSV representa uma linha
    //  do nível. Cada número inteiro representa o tipo de bloco que deve ser criado. Utilize a função CSVHelper::Split
    //  para dividir cada linha do arquivo CSV em números inteiros. A função deve retornar nullptr se o arquivo não
    //  puder ser carregado ou se o número de colunas for diferente do esperado.
    std::ifstream file(fileName);
    if (!file.is_open())
        return nullptr;

    int **levelData = new int *[height];
    std::string line;
    int row = 0;

    while (std::getline(file, line))
    {
        if (row >= height)
        {
            // Mais linhas do que o esperado
            for (int i = 0; i < row; ++i)
                delete[] levelData[i];

            delete[] levelData;
            return nullptr;
        }
        std::vector<int> values = CSVHelper::Split(line);
        if (values.size() != static_cast<size_t>(width))
        {
            // Número de colunas incorreto
            for (int i = 0; i < row; ++i)
                delete[] levelData[i];

            delete[] levelData;
            return nullptr;
        }
        levelData[row] = new int[width];
        for (int col = 0; col < width; ++col)
            levelData[row][col] = values[col];

        ++row;
    }
    if (row != height)
    {
        // Menos linhas do que o esperado
        for (int i = 0; i < row; ++i)
            delete[] levelData[i];

        delete[] levelData;
        return nullptr;
    }
    return levelData;
}

void Game::LoadObjects(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Erro ao abrir o arquivo");
        return;
    }
    nlohmann::json mapData;
    file >> mapData;
    Ground *ground;
    for (const auto &layer: mapData["layers"])
    {
        if (layer["name"] == "Grounds")
        {
            for (const auto &obj: layer["objects"])
            {
                std::string name = obj["name"];
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                float width = static_cast<float>(obj["width"]) * mScale;
                float height = static_cast<float>(obj["height"]) * mScale;
                int id = obj["id"];
                if (name == "Ground")
                {
                    ground = new Ground(this, width, height);
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetSprites();
                }
                else if (name == "Spike")
                {
                    float respawnPositionX = 0.0f;
                    float respawnPositionY = 0.0f;
                    if (obj.contains("properties"))
                        for (const auto &prop: obj["properties"])
                        {
                            std::string propName = prop["name"];
                            if (propName == "RespawnPositionX")
                                respawnPositionX = static_cast<float>(prop["value"]) * mScale;

                            else if (propName == "RespawnPositionY")
                                respawnPositionY = static_cast<float>(prop["value"]) * mScale;
                        }

                    ground = new Ground(this, width, height, true);
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
                    ground->SetSprites();
                }
                else if (name == "Moving Ground")
                {
                    float movingDuration = 0.0f;
                    float speedX = 0.0f;
                    float speedY = 0.0f;

                    if (obj.contains("properties"))
                        for (const auto &prop: obj["properties"])
                        {
                            std::string propName = prop["name"];
                            if (propName == "MovingDuration")
                                movingDuration = prop["value"];

                            else if (propName == "SpeedX")
                                speedX = prop["value"];

                            else if (propName == "SpeedY")
                                speedY = prop["value"];
                        }

                    ground = new Ground(this, width, height, false, true, movingDuration, Vector2(speedX, speedY));
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetSprites();
                }
                else if (name == "Moving Spike")
                {
                    float movingDuration = 0.0f;
                    float speedX = 0.0f;
                    float speedY = 0.0f;
                    float respawnPositionX = 0.0f;
                    float respawnPositionY = 0.0f;

                    if (obj.contains("properties"))
                        for (const auto &prop: obj["properties"])
                        {
                            std::string propName = prop["name"];
                            if (propName == "MovingDuration")
                                movingDuration = prop["value"];

                            else if (propName == "SpeedX")
                                speedX = prop["value"];

                            else if (propName == "SpeedY")
                                speedY = prop["value"];

                            else if (propName == "RespawnPositionX")
                                respawnPositionX = static_cast<float>(prop["value"]) * mScale;

                            else if (propName == "RespawnPositionY")
                                respawnPositionY = static_cast<float>(prop["value"]) * mScale;
                        }

                    ground = new Ground(this, width, height, true, true, movingDuration, Vector2(speedX, speedY));
                    ground->SetId(id);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetRespawPosition(Vector2(respawnPositionX, respawnPositionY));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetSprites();
                }
                else if (name == "DynamicGround")
                {
                    float growSpeedX = 0.0f;
                    float growSpeedY = 0.0f;
                    int growthDirection = 0;
                    float minHeight = 0.0f;
                    float minWidth = 0.0f;

                    if (obj.contains("properties"))
                        for (const auto &prop: obj["properties"])
                        {
                            std::string propName = prop["name"];
                            if (propName == "GrowSpeedX")
                                growSpeedX = static_cast<float>(prop["value"]);

                            else if (propName == "GrowSpeedY")
                                growSpeedY = static_cast<float>(prop["value"]);

                            else if (propName == "GrowthDirection")
                                growthDirection = static_cast<int>(prop["value"]);

                            else if (propName == "MinHeight")
                                minHeight = static_cast<float>(prop["value"]) * mScale;

                            else if (propName == "MinWidth")
                                minWidth = static_cast<float>(prop["value"]) * mScale;

                        }
                    auto* dynamicGround = new DynamicGround(this, minWidth, minHeight, false, false, 0);
                    dynamicGround->SetId(id);
                    dynamicGround->SetMaxWidth(width);
                    dynamicGround->SetMaxHeight(height);
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
                std::vector<int> ids;
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
                    }
                }
                if (target == "DynamicGround" && !grounds.empty()) {
                    ids = ParseIntList(grounds);
                }
                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetGroundsIds(ids);
            }
        }
        if (layer["name"] == "Enemies")
            for (const auto &obj: layer["objects"])
            {
                std::string name = obj["name"];
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                if (name == "Enemy Simple")
                {
                    auto *enemySimple = new EnemySimple(this, 60, 60, 200, 50);
                    enemySimple->SetPosition(Vector2(x, y));
                }
                else if (name == "Flying Enemy")
                {
                    auto *flyingEnemySimple = new FlyingEnemySimple(this, 50, 80, 250, 50);
                    flyingEnemySimple->SetPosition(Vector2(x, y));
                }
                else if (name == "Fox")
                {
                    auto *fox = new Fox(this, 100, 170, 300, 200);
                    fox->SetPosition(Vector2(x, y));
                }
            }
        if (layer["name"] == "Player")
            for (const auto &obj: layer["objects"])
            {
                float x = static_cast<float>(obj["x"]) * mScale;
                float y = static_cast<float>(obj["y"]) * mScale;
                mPlayer = new Player(this, 50, 85);
                mPlayer->SetPosition(Vector2(x, y));
                mPlayer->SetStartingPosition(Vector2(x, y));
            }
    }
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
                        mLogicalWindowWidth = static_cast<float>(mWindowWidth);
                        mLogicalWindowHeight = static_cast<float>(mWindowWidth) / (mOriginalWindowWidth / mOriginalWindowHeight);
                        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
                    }
                    else {
                        mLogicalWindowWidth = static_cast<float>(mWindowHeight) * (mOriginalWindowWidth / mOriginalWindowHeight);
                        mLogicalWindowHeight = static_cast<float>(mWindowHeight);
                        SDL_RenderSetLogicalSize(mRenderer, mLogicalWindowWidth, mLogicalWindowHeight);
                    }
                    const float ratio = mOriginalWindowHeight / static_cast<float>(mLogicalWindowHeight);
                    const int tileSize = static_cast<int>(32 / ratio);
                    mScale = static_cast<float>(tileSize) / 32.0f;
                    ChangeResolution(oldScale);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    mIsPaused = !mIsPaused;

                if (event.key.keysym.sym == SDLK_8)
                    Quit();

                break;

            case SDL_CONTROLLERBUTTONDOWN:
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                    mIsPaused = !mIsPaused;

                break;

            default: ;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    if (!mIsPaused)
    {
        if (mHitstopActive) {}
        else
            for (auto actor: mActors)
                actor->ProcessInput(state, *mController);
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
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    mTicksCount = SDL_GetTicks();

    // Update all actors and pending actors
    if (!mIsPaused)
    {
        if (mHitstopActive)
        {
            if (mHitstopTimer < mHitstopDuration)
                mHitstopTimer += deltaTime;

            else
                mHitstopActive = false;
        }
        else
            UpdateActors(deltaTime);
    }

    if (mResetLevel)
        ResetLevel();

    UpdateCamera(deltaTime);
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

    std::vector<Actor *> deadActors;
    for (auto actor: mActors)
        if (actor->GetState() == ActorState::Destroy)
            deadActors.emplace_back(actor);

    for (auto actor: deadActors)
        delete actor;
}

void Game::UpdateCamera(float deltaTime) { mCamera->Update(deltaTime); }

void Game::AddGround(class Ground *g) { mGrounds.emplace_back(g); }

void Game::RemoveGround(class Ground *g)
{
    auto iter = std::find(mGrounds.begin(), mGrounds.end(), g);
    if (iter != mGrounds.end())
        mGrounds.erase(iter);
}

Ground *Game::GetGroundById(int id) {
    for (Ground* g : mGrounds) {
        if (g->GetId() == id) {
            return g;
        }
    }
    return nullptr;
}

void Game::AddFireBall(class FireBall *f) { mFireBalls.emplace_back(f); }

void Game::RemoveFireball(class FireBall *f)
{
    auto iter = std::find(mFireBalls.begin(), mFireBalls.end(), f);
    if (iter != mFireBalls.end())
        mFireBalls.erase(iter);
}

void Game::AddEnemy(class Enemy *e) { mEnemies.emplace_back(e); }

void Game::RemoveEnemy(class Enemy *e)
{
    auto iter = std::find(mEnemies.begin(), mEnemies.end(), e);
    if (iter != mEnemies.end())
        mEnemies.erase(iter);
}

void Game::AddActor(Actor *actor)
{
    if (mUpdatingActors)
        mPendingActors.emplace_back(actor);

    else
        mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor *actor)
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

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(), [](const DrawComponent *a, const DrawComponent *b)
    {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::GenerateOutput()
{
    // Set draw color to green
    // SDL_SetRenderDrawColor(mRenderer, 0, 88, 105, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    DrawParallaxBackground(); // desenha o fundo com repetição horizontal
    // Ordem de desenho: mais distantes primeiro
    // DrawParallaxLayer(mSky,        0.1f, 0, mWindowHeight / 2);  // camada mais distante
    // DrawParallaxLayer(mMountains,  0.3f, mWindowHeight / 4, mWindowHeight / 3);  // montanhas ao fundo
    // DrawParallaxLayer(mTreesBack,  0.5f, mWindowHeight / 3, mWindowHeight / 2);  // árvores distantes
    // DrawParallaxLayer(mTreesFront, 0.7f, mWindowHeight / 2, mWindowHeight / 2);  // árvores próximas

    for (auto drawable: mDrawables)
        drawable->Draw(mRenderer);

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture *Game::LoadTexture(const std::string &texturePath)
{
    // TODO 4.1 (~4 linhas): Utilize a função `IMG_Load` para carregar a imagem passada como parâmetro
    //  `texturePath`. Esse função retorna um ponteiro para `SDL_Surface*`. Retorne `nullptr` se a
    //  imagem não foi carregada com sucesso.
    SDL_Surface *surface = IMG_Load(texturePath.c_str());
    if (!surface)
    {
        SDL_Log("Falha ao carregar imagem %s: %s", texturePath.c_str(), IMG_GetError());
        return nullptr;
    }

    // TODO 4.2 (~6 linhas): Utilize a função `SDL_CreateTextureFromSurface` para criar uma textura a partir
    //  da imagem carregada anteriormente. Essa função retorna um ponteiro para `SDL_Texture*`. Logo após criar
    //  a textura, utilize a função `SDL_FreeSurface` para liberar a imagem carregada. Se a textura foi carregada
    //  com sucesso, retorne o ponteiro para a textura. Caso contrário, retorne `nullptr`.
    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface); // Libera a superfície, já não é mais necessária

    if (!texture)
    {
        SDL_Log("Falha ao criar textura a partir de %s: %s", texturePath.c_str(), SDL_GetError());
        return nullptr;
    }
    return texture;
}

void Game::Shutdown()
{
    // Delete actors
    while (!mActors.empty())
        delete mActors.back();

    // Delete level data
    if (mLevelData != nullptr)
    {
        for (int i = 0; i < mLevelHeight; ++i)
        {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;
    delete mCamera;

    if (mController)
        SDL_GameControllerClose(mController);

    SDL_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::DrawParallaxBackground()
{
    float parallaxFactor = 0.55f; // fundo se move mais devagar que a câmera

    int bgWidth, bgHeight;
    SDL_QueryTexture(mBackGroundTexture, nullptr, nullptr, &bgWidth, &bgHeight);

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

        SDL_RenderCopy(mRenderer, mBackGroundTexture, nullptr, &dest);
    }
}

void Game::DrawParallaxLayer(SDL_Texture *texture, float parallaxFactor, int y, int h)
{
    int texW, texH;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % texW;
    if (offsetX < 0) offsetX += texW;

    for (int x = -offsetX; x < mLogicalWindowWidth; x += texW)
    {
        SDL_Rect dest = {
            x,
            y,
            texW,
            h
        };

        SDL_RenderCopy(mRenderer, texture, nullptr, &dest);
    }
}

void Game::ResetLevel()
{
    // Delete actors
    while (!mActors.empty())
        delete mActors.back();

    // Delete level data
    if (mLevelData != nullptr)
    {
        for (int i = 0; i < mLevelHeight; ++i)
        {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;
    delete mCamera;

    InitializeActors();

    mResetLevel = false;
}

void Game::ChangeResolution(float oldScale)
{
    mTileSize = mOriginalTileSize * mScale;
    for (auto actor : mActors) {
        actor->ChangeResolution(oldScale, mScale);
    }
    mCamera->ChangeResolution(oldScale, mScale);
    mCamera->SetPosition(Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2, mPlayer->GetPosition().y - mLogicalWindowHeight / 2));

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
