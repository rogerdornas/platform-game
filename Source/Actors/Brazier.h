//
// Created by roger on 20/11/2025.
//

#pragma once
#include "Actor.h"



class Brazier : public  Actor
{
public:
    enum class BrazierState {
        LightOn,
        LightOff
    };

    Brazier(Game* game);

    void OnUpdate(float deltaTime) override;

private:
    void InitLight();
    void ManageAnimations();

    float mWidth;
    float mHeight;
    class Light* mLight;
    class Light* mRedLight;
    BrazierState mBrazierState;

    class ColliderComponent* mAABBComponent;
    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};

