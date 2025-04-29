//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

enum class WallSlideSide {
    notSliding,
    left,
    right
};

class Player : public Actor
{
public:
    Player(Game* game, float width, float height);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    bool GetIsOnGround() { return mIsOnGround; }

private:
    float mHeight;
    float mWidth;
    bool mIsOnGround;

    bool mIsJumping;              // Está no meio de um pulo sustentado?
    float mJumpTime;              // Quanto tempo já pulou
    float mMaxJumpTime;           // Tempo máximo de pulo sustentado
    float mJumpForce;             // Força contínua durante o pulo
    bool mCanJump;
    float mMoveSpeed;
    int mJumpCountInAir;
    int mMaxJumpsInAir;

    class Sword* mSword;
    bool mPrevSwordPressed;
    float mSwordCooldownTimer;
    const float mSwordCooldownDuration;
    float mSwordDirection;

    bool mPrevFireBallPressed;
    float mFireBallCooldownTimer;
    const float mFireBallCooldownDuration;

    bool mCanWallSlide;                         // Habilidade de agarrar na parede
    bool mIsWallSliding;
    WallSlideSide mWallSlideSide;
    float mWallSlideSpeed;
    int mTryingLeavingWallSlideLeft;            // Variaveis para quando tentar sair do wall sliding,
    int mTryingLeavingWallSlideRight;           //
    float mTimerToLeaveWallSlidingLeft;         //
    float mTimerToLeaveWallSlidingRight;        //
    float mMaxTimerToLiveWallSliding;           // ter um tempo para segurar a setinha para se desgrudar da parede

    float mWallJumpTimer;
    float mWallJumpMaxTime;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
    class DashComponent* mDashComponent;
};