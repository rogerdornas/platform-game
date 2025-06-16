//
// Created by roger on 16/04/2025.
//

#pragma once

#include <string>
#include "Actor.h"

class Particle : public Actor
{
public:
    Particle(class Game* game);
    ~Particle() override;

    void OnUpdate(float deltaTime) override;
    void SetSize(float size);
    void SetLifeDuration(float life) { mLifeTDuration = life; }
    void SetIsSplash(bool isSplash) { mIsSplash = isSplash; }
    void SetParticleColor(SDL_Color color) { mColor = color; }
    void SetGravity(bool g) { mGravity = g; }
    void SetSpeedScale(float speedScale) { mSpeedScale = speedScale; }
    void SetDirection(Vector2 direction);

    void Activate();
    void Deactivate();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mSize;
    float mLifeTDuration;
    float mLifeTimer;
    bool mIsSplash;
    std::string mTexturePath;
    SDL_Color mColor;
    bool mGravity;
    float mGravityForce;
    Vector2 mDirection;
    float mSpeedScale;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawParticleComponent* mDrawParticleComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
