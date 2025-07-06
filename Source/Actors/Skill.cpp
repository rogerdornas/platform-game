//
// Created by roger on 30/06/2025.
//

#include "Skill.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Skill::Skill(class Game *game, SkillType skill)
    :Actor(game)
    ,mSkill(skill)
    ,mWidth(64 * mGame->GetScale())
    ,mHeight(64 * mGame->GetScale())
    ,mSkillMessage(nullptr)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    // Componente visual
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 5000);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.5f, mHeight * 2.5f,
                                            "../Assets/Sprites/Skill/Skill.png", "../Assets/Sprites/Skill/Skill.json");

    std::vector idle = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
}

void Skill::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();

    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        SetPlayerSkill();
        LoadSkillMessage();
    }
}

void Skill::SetPlayerSkill() {
    Player* player = mGame->GetPlayer();

    switch (mSkill) {
        case SkillType::Dash:
            player->SetCanDash(true);
            break;

        case SkillType::FireBall:
            player->SetCanFireBall(true);
            break;

        case SkillType::WallSlide:
            player->SetCanWallSlide(true);
            break;

        case SkillType::DoubleJump:
            player->SetMaxJumpsInAir(1);
            break;

        case SkillType::TimeControl:
            break;
    }
}

void Skill::LoadSkillMessage() {
    if (mSkillMessage) {
        return;
    }

    mSkillMessage = new UIScreen(mGame, "../Assets/Fonts/K2D-Bold.ttf");
    mSkillMessage->SetSize(Vector2(mGame->GetLogicalWindowWidth() / 2, mGame->GetLogicalWindowHeight() / 2));
    mSkillMessage->SetPosition(Vector2(mGame->GetLogicalWindowWidth() / 4, mGame->GetLogicalWindowHeight() / 4));

    mSkillMessage->AddImage("../Assets/Sprites/Background/Store.png", Vector2::Zero, mSkillMessage->GetSize());

    UIText* text;
    UIText* skillText;
    int textPointSize = 34;

    text = mSkillMessage->AddText("NOVA HABILIDADE ADQUIRIDA!", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
    if (mSkill == SkillType::TimeControl) {
        text->SetPosition(Vector2((mSkillMessage->GetSize().x - text->GetSize().x) / 2, (mSkillMessage->GetSize().y - text->GetSize().y) * 0.1));
    }
    else {
        text->SetPosition(Vector2((mSkillMessage->GetSize().x - text->GetSize().x) / 2, (mSkillMessage->GetSize().y - text->GetSize().y) / 3));
    }

    switch (mSkill) {
        case SkillType::Dash:
            if (mGame->GetIsPlayingOnKeyboard()) {
                skillText = mSkillMessage->AddText("PRESSIONE [C] PARA USAR DASH", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            }
            else {
                skillText = mSkillMessage->AddText("PRESSIONE [RB] PARA USAR DASH", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            }
            skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 1.2f));
            break;

        case SkillType::FireBall:
            if (mGame->GetIsPlayingOnKeyboard()) {
                skillText = mSkillMessage->AddText("PRESSIONE [A] PARA ATIRAR UMA BOLA DE FOGO", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            }
            else {
                skillText = mSkillMessage->AddText("PRESSIONE [B] PARA ATIRAR UMA BOLA DE FOGO", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            }
                skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 1.2f));
            break;

        case SkillType::WallSlide:
            skillText = mSkillMessage->AddText("AGORA VOCÊ PODE SE AGARRAR NA PAREDE", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 1.2f));
            break;

        case SkillType::DoubleJump:
            skillText = mSkillMessage->AddText("GANHOU UM PULO EXTRA NO AR", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 1.2f));
            break;

        case SkillType::TimeControl:
            skillText = mSkillMessage->AddText("GANHOU CONTROLE SOBRE O TEMPO", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale());
            skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 2.4f));
            skillText = mSkillMessage->AddText("DESEJA VOLTAR NO TEMPO PARA RESTAURAR O EQUILÍBRIO DOS BIOMAS?", Vector2::Zero, Vector2::Zero, textPointSize * mGame->GetScale(), Color::White, mSkillMessage->GetSize().x * 0.8f);
            skillText->SetPosition(Vector2((mSkillMessage->GetSize().x - skillText->GetSize().x) / 2, text->GetPosition().y + text->GetSize().y * 4.2f));
            break;
    }

    if (mSkill == SkillType::TimeControl) {
        Vector2 buttonSize = Vector2(mSkillMessage->GetSize().x * 0.5f, 50 * mScale);
        Vector2 buttonPos = Vector2(mSkillMessage->GetSize().x * 0.25f, mSkillMessage->GetSize().y - buttonSize.y * 2.4f);
        int buttonPointSize = static_cast<int>(34 * mScale);
        mSkillMessage->AddButton("SIM", buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
            [this]() {
                mGame->SetIsPlayingFinalCutscene();
                mSkillMessage->Close();
                mGame->TogglePause();
                SetState(ActorState::Destroy);
            });
        mSkillMessage->AddButton("NÃO", buttonPos + Vector2(0, buttonSize.y * 1.2f), buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() {
            // mGame->SetIsPlayingFinalCutscene();
                mSkillMessage->Close();
                mGame->TogglePause();
                SetState(ActorState::Destroy);
            });
    }

    if (mSkill != SkillType::TimeControl) {
        Vector2 buttonSize = Vector2(mSkillMessage->GetSize().x * 0.5f, 50 * mScale);
        Vector2 buttonPos = Vector2(mSkillMessage->GetSize().x * 0.25f, mSkillMessage->GetSize().y - buttonSize.y * 1.2f);
        int buttonPointSize = static_cast<int>(34 * mScale);
        mSkillMessage->AddButton("VOLTAR", buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() {
            mSkillMessage->Close();
            mGame->TogglePause();
            // CutScene
            std::string cutsceneId;
            switch (mSkill) {
                case SkillType::Dash:
                    cutsceneId = "fimPrimeiroBoss";
                    break;

                case SkillType::FireBall:
                    cutsceneId = "fimPrimeiroBoss";
                    break;

                case SkillType::WallSlide:
                    cutsceneId = "fimTerceiroBoss";
                    break;

                case SkillType::DoubleJump:
                    cutsceneId = "fimQuartoBoss";
                    break;
            }
            auto* cutscene = new Cutscene(mGame, cutsceneId, "../Assets/Cutscenes/Cutscenes.json");
            cutscene->Start();
            mGame->SetCurrentCutscene(cutscene);
            mGame->SetGamePlayState(Game::GamePlayState::Cutscene);
            SetState(ActorState::Destroy);
        });
    }

    mGame->TogglePause();
}

void Skill::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 2.5f);
        mDrawAnimatedComponent->SetHeight(mHeight * 2.5f);
    }

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
