//
// Created by roger on 04/11/2025.
//

#pragma once
#include "Actor.h"

enum class LightType
{
    Point,
    Directional,
    Ambient
};

class Light : public Actor
{
public:
    Light(class Game* game);
    void OnUpdate(float deltaTime) override;

    void SetType(LightType type) { mType = type; }
    LightType GetType() const { return mType; }

    void SetColor(const Vector3& color) { mColor = color; }
    const Vector3& GetColor() const { return mColor; }

    void SetMaxIntensity(float intensity) { mMaxIntensity = intensity; }
    float GetIntensity() const { return mIntensity; }

    void SetRadius(float radius) { mRadius = radius; }
    float GetRadius() const { return mRadius; }

    void Activate(float activateDuration = 0);
    void Deactivate(float deactivateDuration = 0);
    bool IsActivate() const { return mActivate; }

private:
    LightType mType;
    Vector3 mColor;
    float mIntensity;
    float mMaxIntensity;
    float mRadius;
    bool mActivate;
    bool mIsActivating;
    bool mIsDeactivating;
    float mActivateDuration;
    float mActivateTimer;
    float mDeactivateDuration;
    float mDeactivateTimer;
};
