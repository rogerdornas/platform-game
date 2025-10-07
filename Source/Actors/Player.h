//
// Created by roger on 22/04/2025.
//

#pragma once

#include "Actor.h"
#include "HookPoint.h"
#include "../Actors/JumpEffect.h"
#include "../Actors/Sword.h"
#include "../AudioSystem.h"

class Player : public Actor
{
public:
    enum class WallSlideSide { notSliding, left, right };

    Player(Game* game);

    void OnProcessInput(const Uint8* keyState, SDL_GameController &controller) override;
    void OnUpdate(float deltaTime) override;

    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }

    void SetJumpEffects();

    bool GetIsOnGround() const { return mIsOnGround; }
    void SetMaxJumpsInAir(int jumps) { mMaxJumpsInAir = jumps; }
    int GetMaxJumpsInAir() const { return mMaxJumpsInAir; }

    void SetStartingPosition(Vector2 pos) { mStartingPosition = pos; }
    Vector2 GetStartingPosition() const { return mStartingPosition; }

    void ReceiveHit(float damage, Vector2 knockBackDirection);
    void SetCanFireBall(bool canFireBall) { mCanFireBall = canFireBall; }
    bool GetCanFireBall() const { return mCanFireBall; }

    void SetCanWallSlide(bool canWallSlide) { mCanWallSlide = canWallSlide; }
    bool GetCanWallSlide() const { return mCanWallSlide; }

    class Sword* GetSword() const { return mSword; }
    void SetSword() { mSword = new Sword(mGame, this, mSwordWidth, mSwordHeight, 0.15f, mSwordDamage); }
    void SetSwordWidth(float width) { mSwordWidth = width; }
    void SetSwordHeight(float height) { mSwordHeight = height; }
    void SetSwordDamage(float damage) { mSwordDamage = damage; }

    void SetCanDash(bool canDash) { mCanDash = canDash; }
    bool GetCanDash() const { return mCanDash; }

    void SetCanHook(bool canHook) { mCanHook = canHook; }
    bool GetCanHook() const { return mCanHook; }

    void ResetHealthPoints() { mHealthPoints = mMaxHealthPoints; }
    float GetHealthPoints() const { return mHealthPoints; }
    void SetHealthPoints(float HP) { mHealthPoints = HP; }
    float GetMaxHealthPoints() const { return mMaxHealthPoints; }
    void SetMaxHealthPoints(float maxHealthPoints) { mMaxHealthPoints = maxHealthPoints; }
    void SetIsInvulnerable(bool isInvulnerable) { mIsInvulnerable = isInvulnerable; }
    void SetInvulnerableTimer(float invulnerableTimer) { mInvulnerableTimer = invulnerableTimer; }

    void ResetMana() { mMana = mMaxMana; }
    float GetMana() const { return mMana; }
    float GetMaxMana() const { return mMaxMana; }
    void SetMaxMana(float maxMana) { mMaxMana = maxMana; }
    float GetFireballManaCost() const { return mFireballManaCost; }

    void ResetHealCount() { mHealCount = mMaxHealCount; }
    int GetHealCount() const { return mHealCount; }
    void IncreaseHealCount() { mHealCount++; mMaxHealCount++; }

    void IncreaseMoney(int value) { mMoney += value; }
    void DecreaseMoney(int value) { mMoney -= value; }
    int GetMoney() const { return mMoney; }
    void SetMoney(int money) { mMoney = money; }
    int GetSartingMoney() { return mStartMoney; }

    void AdjustSwordAttackSpeed(float factor) { mSwordCooldownDuration /= factor; }

    void SetFireballWidth(float width) { mFireballWidth = width; }
    float GetFireballWidth() const { return mFireballWidth; }
    void SetFireballHeight(float height) { mFireBallHeight = height; }
    float GetFireballHeight() const { return mFireBallHeight; }
    void SetFireballDamage(float damage) { mFireballDamage = damage; }
    float GetFireballDamage() const { return mFireballDamage; }

    void SetDeathCounter(int count) { mDeathCounter = count; }
    int GetDeathCounter() const { return mDeathCounter; }
    void SetIsDead(bool isDead) { mIsDead = isDead; }

    // funções para gerenciar a animação em cutscenes
    void SetIsRunning(bool isRunning) {mIsRunning = isRunning; }

    // Funções para sair da pausa com o controle sem pular ou atirar bola de fogo
    void SetCanJump(bool j) { mCanJump = j; }
    void SetPrevFireBallPressed(bool f) { mPrevFireBallPressed = f; }
    void SetPrevSwordPressed(bool s) { mPrevSwordPressed = s; }

    void SetIsEnteringLevel(Vector2 velocity);

    bool Died();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolveEnemyCollision();

    void ManageAnimations();

    Vector2 mStartingPosition;

    float mWidth;
    float mHeight;

    bool mIsOnGround;
    bool mIsOnMovingGround;
    Vector2 mMovingGroundVelocity;
    float mMoveSpeed;    // Velocidade de movimento

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

    bool mCanWallSlide;                  // Habilidade de agarrar na parede
    bool mIsWallSliding;                 // Se esta deslizando
    WallSlideSide mWallSlideSide;        // Lado que esta deslizando
    float mWallSlideSpeed;               // Velocidade que desce deslizando
    int mTryingLeavingWallSlideLeft;     // Variáveis para quando tentar sair do wall sliding,
    int mTryingLeavingWallSlideRight;    //
    float mTimerToLeaveWallSlidingLeft;  //
    float mTimerToLeaveWallSlidingRight; //
    float mMaxTimerToLiveWallSliding;    // ter um tempo para segurar a setinha para se desgrudar da parede

    bool mIsGoingRight;
    bool mIsGoingLeft;

    float mWallJumpTimer;   // Timer enquanto está pulando de uma parede
    float mWallJumpMaxTime; // Tempo maximo que fica pulando de uma parede

    float mKnockBackSpeed;
    float mKnockBackTimer;
    float mKnockBackDuration;
    float mCameraShakeStrength;

    float mMaxHealthPoints;
    float mHealthPoints;
    bool mIsInvulnerable;
    float mInvulnerableDuration;
    float mInvulnerableTimer;
    int mMaxHealCount;
    int mHealCount;
    float mHealAmount;
    bool mIsHealing;
    float mHealAnimationDuration;
    float mHealAnimationTimer;

    int mMoney;
    int mStartMoney;

    bool mCanHook;
    bool mIsHooking;
    bool mPrevHookPressed;
    Vector2 mHookDirection;
    float mHookSpeed;
    float mHookCooldownDuration;
    float mHookCooldownTimer;
    float mHookingDuration;
    float mHookingTimer;
    Vector2 mHookEnd;
    float mHookAnimProgress;
    bool mIsHookAnimating;
    HookPoint* mHookPoint;
    float mHookAnimationDuration; // duração do efeito visual
    int mHookSegments;
    float mHookAmplitude;
    float mHookSegmentHeight;

    // Atributos para animar sprites
    bool mIsRunning;
    float mHurtDuration;
    float mHurtTimer;
    bool mBlink;
    float mBlinkDuration;
    float mBlinkTimer;

    // Atributos para sounds
    float mRunningSoundIntervalDuration;
    float mRunningSoundIntervalTimer;

    int mDeathCounter;

    bool mIsEnteringLevel;
    float mEnteringLevelDuration;
    float mEnteringLevelTimer;

    float mDeathAnimationDuration;
    float mDeathAnimationTimer;
    bool mIsDead;

    bool mWasOnGround;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;
    class DrawRopeComponent* mDrawRopeComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class ColliderComponent* mAABBComponent;
    class DashComponent* mDashComponent;
};
