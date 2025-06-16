#include "Actor.h"
#include "../Game.h"
#include "../Components/Component.h"
#include <algorithm>

Actor::Actor(Game* game)
    :mGame(game)
    ,mState(ActorState::Active)
    ,mPosition(Vector2::Zero)
    ,mScale(1.0f)
    ,mRotation(0.0f)
{
    mGame->AddActor(this);
}

Actor::~Actor()
{
    mGame->RemoveActor(this);

    for (auto component: mComponents) {
        delete component;
    }
    mComponents.clear();
}

void Actor::Update(float deltaTime)
{
    if (mState == ActorState::Active) {
        for (auto comp: mComponents) {
            comp->Update(deltaTime);
        }

        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime) {}

void Actor::ProcessInput(const Uint8* keyState, SDL_GameController& controller)
{
    if (mState == ActorState::Active) {
        for (auto comp: mComponents) {
            comp->ProcessInput(keyState);
        }

        OnProcessInput(keyState, controller);
    }
}

void Actor::OnProcessInput(const Uint8* keyState, SDL_GameController& controller) {}

void Actor::AddComponent(Component* c)
{
    mComponents.emplace_back(c);
    std::sort(mComponents.begin(), mComponents.end(), [](const Component* a, const Component* b)
    {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });
}

void Actor::RemoveComponent(const class Component* c) {
    auto iter = std::find(mComponents.begin(), mComponents.end(), c);
    mComponents.erase(iter);
}


void Actor::ChangeResolution(float oldScale, float newScale) {}
