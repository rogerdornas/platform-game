//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

enum class WallSlideSide { notSliding, left, right };

class Player : public Actor
{
public:
    Player(Game *game, float width, float height);

    void OnProcessInput(const Uint8 *keyState, SDL_GameController &controller) override;
    void OnUpdate(float deltaTime) override;

    bool GetIsOnGround() const { return mIsOnGround; }

    void SetStartingPosition(Vector2 pos) { mStartingPosition = pos; }
    Vector2 GetStartingPosition() const { return mStartingPosition; }

    void ReceiveHit(float damage, Vector2 knockBackDirection);

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolveEnemyCollision();

    void ManageAnimations();

    bool Died();

    Vector2 mStartingPosition;

    float mHeight;
    float mWidth;

    bool mIsOnGround;
    bool mIsOnMovingGround;
    Vector2 mMovingGroundVelocity;

    bool mIsJumping;     // Está no meio de um pulo sustentado?
    float mJumpTimer;    // Quanto tempo já pulou
    float mMaxJumpTime;  // Tempo máximo de pulo sustentado
    float mJumpForce;    // Força contínua durante o pulo
    bool mCanJump;       // Usado para não continuar pulando ao segurar botão de pular
    float mMoveSpeed;    // Velocidade de movimento
    int mJumpCountInAir; // Numero de pulos realizados no ar
    int mMaxJumpsInAir;  // Maximo de pulos no ar

    bool mCanDash;

    class Sword *mSword;
    bool mPrevSwordPressed;             // Se apertou botão de espada no último frame
    float mSwordCooldownTimer;          // Timer de cooldown da espada
    const float mSwordCooldownDuration; // Cooldown da espada
    float mSwordDirection;              // Director da espada(esquerda, direita, cima, baixo)
    bool mSwordHitEnemy;

    bool mCanFireBall;
    bool mPrevFireBallPressed; // Se apertou botão de fireball no último frame
    float mFireBallCooldownTimer; // Timer de cooldown da fireball
    const float mFireBallCooldownDuration; // Cooldown da fireball
    bool mIsFireAttacking; // As seguintes variáveis são para a feature de ficar parado ao atirar e dar um recoil
    float mStopInAirFireBallTimer; // Timer que fica parado no ar
    const float mStopInAirFireBallMaxDuration; // Cooldown do tempo no ar
    float mFireballRecoil; // Recoil do tiro

    bool mCanWallSlide;                  // Habilidade de agarrar na parede
    bool mIsWallSliding;                 // Se esta deslizando
    WallSlideSide mWallSlideSide;        // Lado que esta deslizando
    float mWallSlideSpeed;               // Velocidade que desce deslizando
    int mTryingLeavingWallSlideLeft;     // Variáveis para quando tentar sair do wall sliding,
    int mTryingLeavingWallSlideRight;    //
    float mTimerToLeaveWallSlidingLeft;  //
    float mTimerToLeaveWallSlidingRight; //
    float mMaxTimerToLiveWallSliding;    // ter um tempo para segurar a setinha para se desgrudar da parede

    float mWallJumpTimer;   // Timer enquanto está pulando de uma parede
    float mWallJumpMaxTime; // Tempo maximo que fica pulando de uma parede

    float mKnockBackSpeed;
    float mKnockBackTimer;
    float mKnockBackDuration;

    float mHealthPoints;
    bool mIsInvulnerable;
    float mInvulnerableDuration;
    float mInvulnerableTimer;

    bool mIsRunning; // Atributos para animar sprites

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
    class DashComponent *mDashComponent;
};
