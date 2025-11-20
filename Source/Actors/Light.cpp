//
// Created by roger on 04/11/2025.
//

#include "Light.h"
#include "../Game.h"

Light::Light(Game* game)
    :Actor(game)
    ,mType(LightType::Point)
    ,mColor(Vector3(1.0f, 1.0f, 1.0f))
    ,mIntensity(0.0f)
    ,mMaxIntensity(1.0f)
    ,mRadius(100.0f)
    ,mActivate(true)
    ,mIsActivating(false)
    ,mIsDeactivating(false)
    ,mActivateDuration(0.0f)
    ,mActivateTimer(0.0f)
    ,mDeactivateDuration(0.0f)
    ,mDeactivateTimer(0.0f)
{
    mGame->GetRenderer()->AddLight(this);
}

void Light::OnUpdate(float deltaTime)
{
    if (mIsActivating) {
        if (mActivateDuration == 0) {
            mIntensity = mMaxIntensity;
        }
        else {
            mIntensity = (mActivateTimer / mActivateDuration) * mMaxIntensity;
        }
        if (mActivateTimer < mActivateDuration) {
            mActivateTimer += deltaTime;
        }
        else {
            mIsActivating = false;
        }
    }

    if (mIsDeactivating) {
        if (mDeactivateDuration == 0) {
            mIntensity = 0;
        }
        else {
            mIntensity = (1 - (mDeactivateTimer / mDeactivateDuration)) * mMaxIntensity;
        }
        if (mDeactivateTimer < mDeactivateDuration) {
            mDeactivateTimer += deltaTime;
        }
        else {
            mActivate = false;
            mIsDeactivating = false;
        }
    }
}

void Light::Activate(float activateDuration) {
    if (mIsActivating) {
        return;
    }
    mActivate = true;
    mIsActivating = true;
    mIsDeactivating = false;
    mActivateTimer = 0;
    mActivateDuration = activateDuration;
}

void Light::Deactivate(float deactivateDuration) {
    if (mIsDeactivating) {
        return;
    }
    mIsDeactivating = true;
    mIsActivating = false;
    mDeactivateTimer = 0;
    mDeactivateDuration = deactivateDuration;
}
