#include "Component.h"
#include "../Actors/Actor.h"

Component::Component(Actor* owner, int updateOrder)
    :mOwner(owner)
    ,mUpdateOrder(updateOrder)
{
    // Add to actor's vector of components
    mOwner->AddComponent(this);
}

Component::~Component() {}

void Component::Update(float deltaTime) {}

void Component::ProcessInput(const Uint8* keyState) {}

class Game* Component::GetGame() const { return mOwner->GetGame(); }
