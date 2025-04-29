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
#include "Components/DrawComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Ground.h"
#include "Actors/ParticleSystem.h"
#include "Actors/Player.h"
#include "Actors/Sword.h"

Game::Game(int windowWidth, int windowHeight, int FPS)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mPlayer(nullptr)
        ,mCamera(nullptr)
        ,mUpdatingActors(false)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mFPS(FPS)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Game-v0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1. (1 linha): Uma biblioteca Random.h foi incluída nesse projeto para a geração de números aleatórios.
    //  Utilize a função Random::Init para inicializar o gerador de números aleatórios (~1 linha).
    Random::Init();

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    InitializeActors();

    mCamera = new Camera(this);

    return true;
}

void Game::InitializeActors()
{
    // --------------
    // TODO - PARTE 3
    // --------------

    mPlayer = new Player(this, 20, 60);
    mPlayer->SetPosition(Vector2(mWindowWidth / 20, mWindowHeight/2));

    for (int i = 0; i < 5; i++) {
        FireBall* fireBall = new FireBall(this);
    }

    Ground* ground1 = new Ground(this, 200, 50);
    ground1->SetPosition(Vector2(mWindowWidth/6, mWindowHeight/10*3));

    Ground* ground2 = new Ground(this, 200, 50);
    ground2->SetPosition(Vector2(300, mWindowHeight/10*7));

    Ground* ground3 = new Ground(this, 200, 50);
    ground3->SetPosition(Vector2(mWindowWidth, mWindowHeight/10*5));

    Ground* ground5 = new Ground(this, 2000, 50);
    ground5->SetPosition(Vector2(mWindowWidth/2, mWindowHeight/10*9));

    Ground* ground6 = new Ground(this, 200, 50);
    ground6->SetPosition(Vector2(700, 100));

    Ground* ground7 = new Ground(this, 200, 50);
    ground7->SetPosition(Vector2(850, 50));

    Ground* ground8 = new Ground(this, 200, 50);
    ground8->SetPosition(Vector2(550, 50));

    Ground* ground9 = new Ground(this, 200, 50);
    ground9->SetPosition(Vector2(1100, -100));

    Ground* ground10 = new Ground(this, 200, 50);
    ground10->SetPosition(Vector2(20, 50));

    Ground* ground11 = new Ground(this, 100, 300);
    ground11->SetPosition(Vector2(500, 620));

    Ground* ground12 = new Ground(this, 3000, 50);
    ground12->SetPosition(Vector2(0, mWindowHeight/10*9));

    Ground* ground13 = new Ground(this, 200, 50);
    ground13->SetPosition(Vector2(-200, -250));

    Ground* ground14 = new Ground(this, 200, 50);
    ground14->SetPosition(Vector2(-200, -450));

    Ground* ground15 = new Ground(this, 200, 50);
    ground15->SetPosition(Vector2(-200, -650));

    Ground* ground16 = new Ground(this, 2, 400);
    ground16->SetPosition(Vector2(-20, 750));

    Ground* ground17 = new Ground(this, 26, 400);
    ground17->SetPosition(Vector2(-500, 750));

    Ground* ground18 = new Ground(this, 30, 400);
    ground18->SetPosition(Vector2(-1000, 750));

    Ground* ground19 = new Ground(this, 200, 50);
    ground19->SetPosition(Vector2(0, 350));

    Ground* ground20 = new Ground(this, 200, 50);
    ground20->SetPosition(Vector2(-300, 240));

    Ground* ground21 = new Ground(this, 1500, 50);
    ground21->SetPosition(Vector2(2800, 700));

    Ground* ground22 = new Ground(this, 100, 1000);
    ground22->SetPosition(Vector2(2800, 400));

    Ground* ground23 = new Ground(this, 100, 1000);
    ground23->SetPosition(Vector2(3000, 400));

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
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    mWindowWidth = event.window.data1;
                    mWindowHeight = event.window.data2;
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 1000.0 / mFPS));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    // Update all actors and pending actors
    UpdateActors(deltaTime);
    UpdateCamera(deltaTime);
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }

}

void Game::UpdateCamera(float deltatime) {
    mCamera->Update(deltatime);
}


void Game::AddGround(class Ground* g) {
    mGrounds.emplace_back(g);
}


void Game::RemoveGround(class Ground *g) {
    auto iter = std::find(mGrounds.begin(), mGrounds.end(), g);
    if (iter != mGrounds.end()) {
        mGrounds.erase(iter);
    }
}

void Game::AddFireBall(class FireBall *f) {
    mFireBalls.emplace_back(f);
}

void Game::RemoveFireball(class FireBall *f) {
    auto iter = std::find(mFireBalls.begin(), mFireBalls.end(), f);
    if (iter != mFireBalls.end()) {
        mFireBalls.erase(iter);
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
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


void Game::GenerateOutput()
{
    // Set draw color to black
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }
    delete mCamera;
    delete mPlayer;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
