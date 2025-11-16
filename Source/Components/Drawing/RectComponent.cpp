//
// Created by Lucas N. Ferreira on 09/10/25.
//

#include "RectComponent.h"
#include "../../Renderer/Renderer.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Actors/DynamicGround.h"

RectComponent::RectComponent(class Actor* owner, int width, int height, RendererMode mode, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mMode(mode)
    , mWidth(width)
    , mHeight(height)
{

}

RectComponent::~RectComponent()
{

}

void RectComponent::Draw(class Renderer *renderer)
{
    if(mIsVisible) {
        renderer->DrawRect(mOwner->GetPosition(), Vector2(mWidth, mHeight), mOwner->GetTransformRotation(),
                           mColor, GetGame()->GetCamera()->GetPosCamera(), mMode);
    }
}