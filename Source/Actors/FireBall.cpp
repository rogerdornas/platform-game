//
// Created by roger on 29/04/2025.
//

#include "FireBall.h"

#include "Light.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

FireBall::FireBall(class Game* game)
    :Actor(game)
    ,mWidth(80.0f * mGame->GetScale())
    ,mHeight(80.0f * mGame->GetScale())
    ,mSpeed(1800.0f * mGame->GetScale())
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
    ,mThrowingWidth(mWidth)
    ,mExplodingWidth(mWidth * 1.3f)
    ,mFireballState(State::Exploding)
    ,mFireballOffscreenLimit(0.2f)
    ,mDeactivateDuration(0.3f)
    ,mDeactivateTimer(mDeactivateDuration)
    ,mDamage(20)
    ,mIsFromEnemy(false)
    ,mLight(nullptr)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
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

    mDrawComponent = new AnimatorComponent(this,
                                            "../Assets/Sprites/Fireball2/Fireball.png",
                                            "../Assets/Sprites/Fireball2/Fireball.json",
                                             mWidth * 1.8f, mHeight * 1.8f, 1001);

    std::vector<int> firing = {0, 1, 2, 3};
    mDrawComponent->AddAnimation("firing", firing);

    std::vector<int> explosion = {5, 6, 7, 8, 4, 4};
    mDrawComponent->AddAnimation("explosion", explosion);

    const std::vector end = {4};
    mDrawComponent->AddAnimation("end", end);

    mDrawComponent->SetAnimation("firing");
    const float fps = 4.0f / mDeactivateDuration;
    mDrawComponent->SetAnimFPS(fps);
    // mDrawAnimatedComponent->UseRotation(true);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    // mLight = new Light(mGame);
    // mLight->SetRadius(100.0f);
    // mLight->SetIntensity(0.95f);
    // mLight->SetColor(Vector3(0.92f, 0.37f, 0.37f));
    // mLight->SetActivate(false);

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
                if (mDrawComponent) {
                    // mDrawAnimatedComponent->ResetAnimationTimer();
                    mDrawComponent->SetAnimation("explosion");
                }
                mWidth = mExplodingWidth;
                mHeight = mExplodingWidth;
                if (mDrawComponent) {
                    mDrawComponent->SetWidth(mWidth * 1.8f);
                    mDrawComponent->SetHeight(mHeight * 1.8f);
                }
                mFireballState = State::Exploding;
            }
            ResolveGroundCollision();
            ResolveEnemyCollision();
            ResolvePlayerCollision();

            // Verifica se fireball está fora da tela mais um intervalo
            if (!IsOnScreen()) {
                if (mDrawComponent) {
                    // mDrawAnimatedComponent->ResetAnimationTimer();
                    mDrawComponent->SetAnimation("explosion");
                }
                mWidth = mExplodingWidth;
                mHeight = mExplodingWidth;
                if (mDrawComponent) {
                    mDrawComponent->SetWidth(mWidth * 1.8f);
                    mDrawComponent->SetHeight(mHeight * 1.8f);
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
            if (mLight) {
                // mLight->SetActivate(false);
                mLight->Deactivate(0.7f);
            }
            break;
    }
    if (mLight) {
        mLight->SetPosition(GetPosition());
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
    return (GetPosition().x < mGame->GetCamera()->GetPosCamera().x + mGame->GetRenderer()->GetZoomedWidth()  + mGame->GetRenderer()->GetZoomedWidth() * mFireballOffscreenLimit &&
            GetPosition().x > mGame->GetCamera()->GetPosCamera().x  - mGame->GetRenderer()->GetZoomedWidth() * mFireballOffscreenLimit &&
            GetPosition().y > mGame->GetCamera()->GetPosCamera().y - mGame->GetRenderer()->GetZoomedHeight() * mFireballOffscreenLimit &&
            GetPosition().y < mGame->GetCamera()->GetPosCamera().y + mGame->GetRenderer()->GetZoomedHeight() + mGame->GetRenderer()->GetZoomedHeight() * mFireballOffscreenLimit);
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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    mAABBComponent->SetActive(true); // reativa colisão
    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
        mRectComponent->SetVisible(true);
    }
    if (mDrawComponent) {
        mDrawComponent->SetWidth(mWidth * 1.8f);
        mDrawComponent->SetHeight(mHeight * 1.8f);
        mDrawComponent->SetVisible(true);
        mDrawComponent->SetAnimation("firing");
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
    if (!mSound.IsValid()) {
        mSound = mGame->GetAudio()->PlaySound("FireBall/ShootFireBall.wav");
    }
    mFireballState = State::Throwing;

    if (!mLight) {
        mLight = new Light(mGame);
        mLight->SetRadius(250.0f);
        mLight->SetMaxIntensity(1.2f);
        mLight->SetColor(Vector3(0.72f, 0.37f, 0.37f));
    }
    if (mLight) {
        mLight->Activate(0.1f);
    }
}

void FireBall::Deactivate() {
    mIsFromEnemy = false;
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mAABBComponent->SetActive(false); // desativa colisão
    mFireballState = State::Deactivate;
    mDurationTimer = 0;
    mDeactivateTimer = 0;
    mDamage = 20;

    if (mRectComponent) {
        mRectComponent->SetVisible(false);
    }
    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
        mDrawComponent->SetAnimation("end");
    }
    SetState(ActorState::Paused);
    mSound.Reset();
    if (mLight) {
        mLight->Deactivate();
    }
}

void FireBall::ResolveGroundCollision() {
    std::vector<Ground*> grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                if (mSound.IsValid()) {
                    if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
                        mGame->GetAudio()->StopSound(mSound);
                    }
                    // Verifica se fireball está dentro da tela mais um intervalo
                    if (IsOnScreen()) {
                        mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
                    }
                }
                if (mDrawComponent) {
                    // mDrawAnimatedComponent->ResetAnimationTimer();
                    mDrawComponent->SetAnimation("explosion");
                }
                mFireballState = State::Exploding;
                mWidth = mExplodingWidth;
                mHeight = mExplodingWidth;
                if (mDrawComponent) {
                    mDrawComponent->SetWidth(mWidth * 1.8f);
                    mDrawComponent->SetHeight(mHeight * 1.8f);
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
                if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
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
                    if (mDrawComponent) {
                        // mDrawAnimatedComponent->ResetAnimationTimer();
                        mDrawComponent->SetAnimation("explosion");
                    }
                    mFireballState = State::Exploding;
                    mWidth = mExplodingWidth;
                    mHeight = mExplodingWidth;
                    if (mDrawComponent) {
                        mDrawComponent->SetWidth(mWidth * 1.8f);
                        mDrawComponent->SetHeight(mHeight * 1.8f);
                    }
                }
            }
        }
    }
}

void FireBall::ResolvePlayerCollision() {
    if (mIsFromEnemy) {
        Player* player = mGame->GetPlayer();
        if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
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
            if (mDrawComponent) {
                // mDrawAnimatedComponent->ResetAnimationTimer();
                mDrawComponent->SetAnimation("explosion");
            }
            mFireballState = State::Exploding;
            mWidth = mExplodingWidth;
            mHeight = mExplodingWidth;
            if (mDrawComponent) {
                mDrawComponent->SetWidth(mWidth * 1.8f);
                mDrawComponent->SetHeight(mHeight * 1.8f);
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
    //     mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
