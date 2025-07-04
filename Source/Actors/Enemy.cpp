//
// Created by roger on 02/05/2025.
//

#include "Enemy.h"
#include "Effect.h"
#include "Money.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Enemy::Enemy(Game* game, float width, float height, float moveSpeed, float heathPoints, float contactDamage)
    :Actor(game)
    ,mWidth(width * mGame->GetScale())
    ,mHeight(height * mGame->GetScale())
    ,mMoneyDrop(1)
    ,mMoveSpeed(moveSpeed * mGame->GetScale())
    ,mHealthPoints(heathPoints)
    ,mMaxHealthPoints(heathPoints)
    ,mContactDamage(contactDamage)
    ,mKnockBackSpeed(0.0f)
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.0f)
    ,mCameraShakeStrength(60.0f * mGame->GetScale())
    ,mIsFlashing(false)
    ,mFlashDuration(0.07f)
    ,mFlashTimer(mFlashDuration)
    ,mPlayerSpotted(false)
    ,mOffscreenLimit(0.2f)
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

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {245, 154, 25, 255});
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 40000 * mGame->GetScale());
    mAABBComponent = new AABBComponent(this, v1, v3);

    game->AddEnemy(this);
}

Enemy::~Enemy() {
    mGame->RemoveEnemy(this);
}

void Enemy::ReceiveHit(float damage, Vector2 knockBackDirection) {
    mHealthPoints -= damage;
    mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + knockBackDirection * mKnockBackSpeed);
    mKnockBackTimer = 0;
    mIsFlashing = true;
    mFlashTimer = 0;
    mPlayerSpotted = true;

    auto* blood = new ParticleSystem(mGame, 10, 170.0, 3.0, 0.07f);
    blood->SetPosition(GetPosition());
    blood->SetEmitDirection(knockBackDirection);
    blood->SetParticleSpeedScale(1);
    blood->SetParticleColor(SDL_Color{226, 90, 70, 255});
    blood->SetParticleGravity(true);

    auto* circleBlur = new Effect(mGame);
    circleBlur->SetDuration(0.3);
    circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 3.5f);
    circleBlur->SetEnemy(*this);
    circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
    circleBlur->SetEffect(TargetEffect::Circle);

    if (IsOnScreen()) {
        mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
    }
}

bool Enemy::Died() {
    if (mHealthPoints <= 0) {
        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("KillEnemy/KillEnemy1.wav");
        }

        std::vector<Money*> moneys = mGame->GetMoneys();

        // Primeiro dropa 1 dinheiro de cada
        if (mMoneyDrop >= 1) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Small) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 1;
                    break;
                }
            }
        }
        if (mMoneyDrop >= 5) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Medium) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 5;
                    break;
                }
            }
        }
        if (mMoneyDrop >= 10) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Large) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 10;
                    break;
                }
            }
        }

        // Cria grandes
        while (mMoneyDrop >= 10) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Large) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 10;
                    break;
                }
            }
        }

        // Cria médios
        while (mMoneyDrop >= 5) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Medium) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 5;
                    break;
                }
            }
        }

        // Cria pequenos
        while (mMoneyDrop >= 1) {
            for (Money* m: moneys) {
                if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Small) {
                    m->SetState(ActorState::Active);
                    m->SetPosition(GetPosition());
                    mMoneyDrop -= 1;
                    break;
                }
            }
        }

        return true;
    }
    return false;
}

void Enemy::ResolveEnemyCollision() const {
    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e: enemies) {
            if (e != this) {
                if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                    mAABBComponent->ResolveCollision(*e->GetComponent<AABBComponent>());
                }
            }
        }
    }
}

void Enemy::ResolveGroundCollision() {
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    std::array<bool, 4> collisionSide{};
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                    // Colidiu top
                    if (collisionSide[0]) {
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionSide[1]) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionSide[2]) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionSide[3]) {
                        ReceiveHit(10, Vector2::UnitX);
                    }

                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

bool Enemy::IsOnScreen() {
    return (GetPosition().x < mGame->GetCamera()->GetPosCamera().x + mGame->GetLogicalWindowWidth()  + mGame->GetLogicalWindowWidth() * mOffscreenLimit &&
            GetPosition().x > mGame->GetCamera()->GetPosCamera().x  - mGame->GetLogicalWindowWidth() * mOffscreenLimit &&
            GetPosition().y > mGame->GetCamera()->GetPosCamera().y - mGame->GetLogicalWindowHeight() * mOffscreenLimit &&
            GetPosition().y < mGame->GetCamera()->GetPosCamera().y + mGame->GetLogicalWindowHeight() + mGame->GetLogicalWindowHeight() * mOffscreenLimit);
}