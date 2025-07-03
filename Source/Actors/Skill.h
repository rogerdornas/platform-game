//
// Created by roger on 30/06/2025.
//

#pragma once

#include "Actor.h"
#include "../UIElements/UIScreen.h"

class Skill : public Actor
{
public:
    enum class SkillType {
        Dash,
        FireBall,
        WallSlide,
        DoubleJump,
        TimeControl
    };

    Skill(class Game* game, SkillType skill);

    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    SkillType mSkill;
    float mWidth;
    float mHeight;
    UIScreen* mSkillMessage;

    void SetPlayerSkill();
    void LoadSkillMessage();

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;
    class AABBComponent* mAABBComponent;
};
