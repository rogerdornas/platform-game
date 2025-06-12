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

FireBall::FireBall(class Game *game)
    :Actor(game)
    ,mWidth(80.0f * mGame->GetScale())
    ,mHeight(80.0f * mGame->GetScale())
    ,mSpeed(1800.0f * mGame->GetScale())
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
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
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Koopa/Shell.png", mWidth + 30 * mGame->GetScale(), mHeight + 30 * mGame->GetScale());

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.0, mHeight * 2.0, "../Assets/Sprites/Fireball/Fireball.png", "../Assets/Sprites/Fireball/Fireball.json", 999);
    std::vector<int> firing = {0, 1, 2, 3, 4};
    mDrawAnimatedComponent->AddAnimation("firing", firing);

    mDrawAnimatedComponent->SetAnimation("firing");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddFireBall(this);
}

FireBall::~FireBall() {
    mGame->RemoveFireball(this);
}

void FireBall::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        Deactivate();
    }
    else {
        Activate();
        ResolveGroundCollision();
        ResolveEnemyCollision();
        ResolvePlayerCollision();
    }
    ManageAnimations();
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
        mDrawSpriteComponent->SetWidth(mWidth * 2.0);
        mDrawSpriteComponent->SetHeight(mHeight * 2.0);
        mDrawSpriteComponent->SetIsVisible(true);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 2.0);
        mDrawAnimatedComponent->SetHeight(mHeight * 2.0);
        mDrawAnimatedComponent->SetIsVisible(true);
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
    if (!mSound.IsValid()) {
        mSound = mGame->GetAudio()->PlaySound("FireBall/ShootFireBall.wav");
    }
}

void FireBall::Deactivate() {
    mIsFromEnemy = false;
    SetState(ActorState::Paused);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mDurationTimer = 0;
    mAABBComponent->SetActive(false); // desativa colisão

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(false);
    }

    auto* explosion = new ParticleSystem(mGame, mWidth / 5 / mGame->GetScale(), 200.0, 0.2, 0.07f);
    explosion->SetPosition(GetPosition() + GetForward() * (mWidth / 2));
    explosion->SetEmitDirection(Vector2::Zero);
    explosion->SetIsSplash(true);
    explosion->SetParticleSpeedScale(mWidth / 50 / mGame->GetScale());
    explosion->SetParticleColor(SDL_Color{247, 118, 34, 255});
    explosion->SetParticleGravity(false);
    if (mSound.IsValid()) {
        if (mGame->GetAudio()->GetSoundState(mSound) == SoundState::Playing) {
            mGame->GetAudio()->StopSound(mSound);
        }
        // Verifica se fireball está dentro da tela
        if (GetPosition().x < mGame->GetCamera()->GetPosCamera().x + mGame->GetLogicalWindowWidth() &&
        GetPosition().x > mGame->GetCamera()->GetPosCamera().x &&
        GetPosition().y > mGame->GetCamera()->GetPosCamera().y &&
        GetPosition().y < mGame->GetCamera()->GetPosCamera().y + mGame->GetLogicalWindowHeight()
        )
        {
            mGame->GetAudio()->PlaySound("FireBall/ExplodeFireBall.wav");
        }
        mSound.Reset();
    }
}

void FireBall::ResolveGroundCollision() {
    std::vector<Ground*> grounds;
    grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                Deactivate();
            }
        }
    }
}

void FireBall::ResolveEnemyCollision() {
    if (!mIsFromEnemy) {
        std::vector<Enemy*> enemys;
        enemys = mGame->GetEnemies();
        if (!enemys.empty()) {
            for (Enemy* e : enemys) {
                if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                    e->ReceiveHit(mDamage, GetForward());
                    Deactivate();
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
            Deactivate();
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

    mDrawAnimatedComponent->SetWidth(mWidth * 2.0f);
    mDrawAnimatedComponent->SetHeight(mHeight * 2.0f);

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
