//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "DrawComponent.h"
#include "../../Game.h"
#include "../../Actors/Actor.h"

DrawComponent::DrawComponent(class Actor *owner, int drawOrder)
        : Component(owner)
        , mDrawOrder(drawOrder)
        , mIsVisible(true)
        , mColor(Vector3::One)
        , mAlpha(1.0f)
{
    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    mOwner->GetGame()->RemoveDrawable(this);
}

void DrawComponent::Draw(Renderer *renderer)
{

}
