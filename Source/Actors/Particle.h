//
// Created by roger on 16/04/2025.
//

#pragma once
#include "Actor.h"

class Particle : public Actor
{
public:
    Particle(class Game *game, float lifeTime = 10.0f);
    void OnUpdate(float deltaTime) override;
    void SetLife(float life) { mDeathTimer = life; }

private:
    float mDeathTimer;

    // class DrawComponent* mDrawComponent;
    class RigidBodyComponent *mRigidBodyComponent;
};
