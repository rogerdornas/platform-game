//
// Created by roger on 29/04/2025.
//

#include "FireBall.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

FireBall::FireBall(class Game* game)
    :Actor(game)
    ,mWidth(80.0f * mGame->GetScale())
    ,mHeight(80.0f * mGame->GetScale())
    ,mSpeed(1800.0f * mGame->GetScale())
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
    ,mFireballState(State::Exploding)
    ,mFireballOffscreenLimit(0.2f)
    ,mDeactivateDuration(0.3f)
    ,mDeactivateTimer(mDeactivateDuration)
    ,mDamage(20)
    ,mIsFromEnemy(false)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {37, 218, 255, 255});

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.8f, mHeight * 1.8f, "../Assets/Sprites/Fireball2/Fireball.png", "../Assets/Sprites/Fireball2/Fireball.json", 1001);

    std::vector<int> firing = {0, 1, 2, 3};
    mDrawAnimatedComponent->AddAnimation("firing", firing);

    std::vector<int> explosion = {5, 6, 7, 8, 4, 4};
    mDrawAnimatedComponent->AddAnimation("explosion", explosion);

    const std::vector end = {4};
    mDrawAnimatedComponent->AddAnimation("end", end);

    mDrawAnimatedComponent->SetAnimation("firing");
    const float fps = 4.0f / mDeactivateDuration;
    mDrawAnimatedComponent->SetAnimFPS(fps);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddFireBall(this);
}

FireBall::~FireBall() {
    mGame->RemoveFireball(this);
}

void FireBall::OnUpdate(float deltaTime) {
    switch (mFireballState) {
        case State::Deactivate:
            Activate();
            break;
        case State::Throwing:
            mDurationTimer += deltaTime;
            if (mDurationTimer >= mDuration) {
                if (mSound.IsValid()) {
                    if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
                        mGame->GetAudio()->StopSound(mSound);
                    }
                    // Verifica se fireball está dentro da tela mais um intervalo
                    if (IsOnScreen()) {
                        mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
                    }
                }
                mDrawAnimatedComponent->ResetAnimationTimer();
                mDrawAnimatedComponent->SetAnimation("explosion");
                mWidth *= 1.5;
                mHeight *= 1.5;
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                    mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
                }
                mFireballState = State::Exploding;
            }
            ResolveGroundCollision();
            ResolveEnemyCollision();
            ResolvePlayerCollision();

            // Verifica se fireball está fora da tela mais um intervalo
            if (!IsOnScreen()) {
                mDrawAnimatedComponent->ResetAnimationTimer();
                mDrawAnimatedComponent->SetAnimation("explosion");
                mWidth *= 1.5;
                mHeight *= 1.5;
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                    mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
                }
                mFireballState = State::Exploding;
            }
            break;

        case State::Exploding:
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mAABBComponent->SetActive(false); // desativa colisão
            mDeactivateTimer += deltaTime;
            if (mDeactivateTimer >= mDeactivateDuration) {
                Deactivate();
            }
            break;
    }
}

void FireBall::ExplosionEffect() {
    auto* explosion = new ParticleSystem(mGame, mWidth / 5 / mGame->GetScale(), 200.0, 0.2, 0.07f);
    explosion->SetPosition(GetPosition() + GetForward() * (mWidth / 2));
    explosion->SetEmitDirection(Vector2::Zero);
    explosion->SetIsSplash(true);
    explosion->SetParticleSpeedScale(mWidth / 50 / mGame->GetScale());
    explosion->SetParticleColor(SDL_Color{247, 118, 34, 255});
    explosion->SetParticleGravity(false);
}

bool FireBall::IsOnScreen() {
    return (GetPosition().x < mGame->GetCamera()->GetPosCamera().x + mGame->GetLogicalWindowWidth()  + mGame->GetLogicalWindowWidth() * mFireballOffscreenLimit &&
            GetPosition().x > mGame->GetCamera()->GetPosCamera().x  - mGame->GetLogicalWindowWidth() * mFireballOffscreenLimit &&
            GetPosition().y > mGame->GetCamera()->GetPosCamera().y - mGame->GetLogicalWindowHeight() * mFireballOffscreenLimit &&
            GetPosition().y < mGame->GetCamera()->GetPosCamera().y + mGame->GetLogicalWindowHeight() + mGame->GetLogicalWindowHeight() * mFireballOffscreenLimit);
}


void FireBall::Activate() {
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
        mDrawPolygonComponent->SetIsVisible(true);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth * 1.8f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.8f);
        mDrawSpriteComponent->SetIsVisible(true);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
        mDrawAnimatedComponent->SetIsVisible(true);
        mDrawAnimatedComponent->SetAnimation("firing");
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
    if (!mSound.IsValid()) {
        mSound = mGame->GetAudio()->PlaySound("FireBall/ShootFireBall.wav");
    }
    mFireballState = State::Throwing;
}

void FireBall::Deactivate() {
    mIsFromEnemy = false;
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mAABBComponent->SetActive(false); // desativa colisão
    mFireballState = State::Deactivate;
    mDurationTimer = 0;
    mDeactivateTimer = 0;
    mDamage = 20;

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(false);
        mDrawAnimatedComponent->SetAnimation("end");
    }
    SetState(ActorState::Paused);
    mSound.Reset();
}

void FireBall::ResolveGroundCollision() {
    std::vector<Ground*> grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                if (mSound.IsValid()) {
                    if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
                        mGame->GetAudio()->StopSound(mSound);
                    }
                    // Verifica se fireball está dentro da tela mais um intervalo
                    if (IsOnScreen()) {
                        mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
                    }
                }
                mDrawAnimatedComponent->ResetAnimationTimer();
                mDrawAnimatedComponent->SetAnimation("explosion");
                mFireballState = State::Exploding;
                mWidth *= 1.5;
                mHeight *= 1.5;
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                    mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
                }
            }
        }
    }
}

void FireBall::ResolveEnemyCollision() {
    if (!mIsFromEnemy) {
        std::vector<Enemy*> enemies = mGame->GetEnemies();
        if (!enemies.empty()) {
            for (Enemy* e : enemies) {
                if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                    e->ReceiveHit(mDamage, GetForward());
                    if (mSound.IsValid()) {
                        if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
                            mGame->GetAudio()->StopSound(mSound);
                        }
                        // Verifica se fireball está dentro da tela mais um intervalo
                        if (IsOnScreen()) {
                            mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
                        }
                    }
                    mDrawAnimatedComponent->ResetAnimationTimer();
                    mDrawAnimatedComponent->SetAnimation("explosion");
                    mFireballState = State::Exploding;
                    mWidth *= 1.5;
                    mHeight *= 1.5;
                    if (mDrawAnimatedComponent) {
                        mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                        mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
                    }
                }
            }
        }
    }
}

void FireBall::ResolvePlayerCollision() {
    if (mIsFromEnemy) {
        Player* player = mGame->GetPlayer();
        if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
            player->ReceiveHit(mDamage, GetForward());
            if (mSound.IsValid()) {
                if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
                    mGame->GetAudio()->StopSound(mSound);
                }
                // Verifica se fireball está dentro da tela mais um intervalo
                if (IsOnScreen()) {
                    mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
                }
            }
            mDrawAnimatedComponent->ResetAnimationTimer();
            mDrawAnimatedComponent->SetAnimation("explosion");
            mFireballState = State::Exploding;
            mWidth *= 1.5;
            mHeight *= 1.5;
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
            }
        }
    }
}

void FireBall::ManageAnimations() {

}

void FireBall::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mSpeed = mSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
    mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
