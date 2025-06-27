//
// Created by roger on 09/06/2025.
//

#pragma once

#include "Actor.h"

class Projectile : public Actor
{
public:
    enum class ProjectileType {
        Acid,
        OrangeBall
    };

    Projectile(class Game *game, ProjectileType type = ProjectileType::Acid, float width = 0, float height = 0, float speed = 0, float damage = 0);
    ~Projectile();

    void OnUpdate(float deltaTime) override;

    float GetWidth() override { return mWidth; }
    void SetWidth(float w) { mWidth = w; }
    void SetHeight(float h) { mHeight = h; }
    void SetSpeed(float s) { mSpeed = s; }
    void SetDamage(float d) { mDamage = d; }

    ProjectileType GetProjectileType() const { return mProjectileType; }

    void Activate();
    void Deactivate();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolvePlayerCollision();

    void ExplosionEffect();

    ProjectileType mProjectileType;
    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mDurationTimer;

    float mDamage;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
