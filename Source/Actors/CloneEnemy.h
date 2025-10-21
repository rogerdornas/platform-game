//
// Created by roger on 09/10/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"
#include "../Actors/JumpEffect.h"
#include "../Actors/Sword.h"

class CloneEnemy : public Enemy
{
public:
    struct InputCommand {
        bool left = false;
        bool leftSlow = false;
        bool right = false;
        bool rightSlow = false;
        bool up = false;
        bool down = false;
        bool lookUp = false;
        bool lookDown = false;
        bool jump = false;
        bool dash = false;
        bool sword = false;
        bool fireBall = false;
        bool heal = false;
        float delay = 0.5;
    };

    CloneEnemy(Game* game);

    void OnUpdate(float deltaTime) override;
    void OnProcessInput(const Uint8 *keyState, SDL_GameController &controller) override;

    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void ResolvePlayerCollision();
    void ManageAnimations();

    std::vector<InputCommand> mInputCommands;

    bool mIsOnGround;
    bool mIsOnMovingGround;
    Vector2 mMovingGroundVelocity;

    float mMaxTimeOutOfGroundToJump;  // Variáveis para dar um intervalo para pular quando sair do chão ou parede
    float mTimerOutOfGroundToJump;
    float mMaxTimeOutOfWallToJump;
    float mTimerOutOfWallToJump;

    bool mIsJumping;     // Está no meio de um pulo sustentado?
    float mJumpTimer;    // Quanto tempo já pulou
    float mMaxJumpTime;  // Tempo máximo de pulo sustentado
    float mJumpForce;    // Força contínua durante o pulo
    bool mCanJump;       // Usado para não continuar pulando ao segurar botão de pular
    int mJumpCountInAir; // Numero de pulos realizados no ar
    int mMaxJumpsInAir;  // Maximo de pulos no ar
    float mLowGravity;
    float mMediumGravity;
    float mHighGravity;
    std::vector<JumpEffect*> mJumpEffects;

    bool mCanDash;

    Sword* mSword;
    bool mPrevSwordPressed;             // Se apertou botão de espada no último frame
    float mSwordCooldownTimer;          // Timer de cooldown da espada
    float mSwordCooldownDuration;       // Cooldown da espada
    float mSwordWidth;
    float mSwordHeight;
    float mSwordDamage;
    float mSwordDirection;              // Director da espada(esquerda, direita, cima, baixo)
    bool mSwordHitEnemy;
    bool mSwordHitGround;
    bool mSwordHitSpike;
    bool mSwordHitPlayer;

    bool mCanFireBall;
    bool mPrevFireBallPressed; // Se apertou botão de fireball no último frame
    float mFireBallCooldownTimer; // Timer de cooldown da fireball
    const float mFireBallCooldownDuration; // Cooldown da fireball
    bool mIsFireAttacking; // As seguintes variáveis são para a feature de ficar parado ao atirar e dar um recoil
    float mStopInAirFireBallTimer; // Timer que fica parado no ar
    const float mStopInAirFireBallMaxDuration; // Cooldown do tempo no ar
    float mFireballRecoil; // Recoil do tiro
    float mFireballWidth;
    float mFireBallHeight;
    float mFireballSpeed;
    float mFireballDamage;
    float mMaxMana;
    float mMana;
    float mManaIncreaseRate;
    float mFireballManaCost;
    float mFireballAnimationDuration;
    float mFireballAnimationTimer;

    float mHealAmount;
    bool mIsHealing;
    float mHealAnimationDuration;
    float mHealAnimationTimer;

    // Atributos para animar sprites
    bool mIsRunning;
    float mStopRunningDuration; // Para não bugar a anamiação de correr
    float mStopRunningTimer;
    float mHurtDuration;
    bool mBlink;
    float mBlinkDuration;
    float mBlinkTimer;

    // Atributos para sounds
    float mRunningSoundIntervalDuration;
    float mRunningSoundIntervalTimer;

    bool mWasOnGround;

    class DashComponent* mDashComponent;
};
