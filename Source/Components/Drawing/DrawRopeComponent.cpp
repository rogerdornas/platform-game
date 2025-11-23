//
// Created by roger on 23/11/2025.
//

#include "DrawRopeComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"

DrawRopeComponent::DrawRopeComponent(Actor* owner, const std::string& texturePath, int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mStart(Vector2::Zero)
    ,mEnd(Vector2::Zero)
    ,mAnimProgress(0.0f)
    ,mSegments(20)
    ,mAmplitude(10.0f)
    ,mSegmentHeight(8.0f)
{
    mTexture = GetGame()->GetRenderer()->GetTexture(texturePath);
}

DrawRopeComponent::~DrawRopeComponent() {

}

void DrawRopeComponent::SetEndpoints(Vector2 start, Vector2 end) {
    mStart = start;
    mEnd = end;
}

void DrawRopeComponent::Draw(Renderer* renderer) {
    if (!mIsVisible) {
        return;
    }

    if (mAnimProgress >= 1.0f && mAnimProgress <= 0.0f) {
        return;
    }

    Vector2 lastPos = mStart;

    // Vetor direção total
    Vector2 totalDelta = mEnd - mStart;

    // Frequência da onda na corda
    float frequency = 4.0f;

    for (int i = 1; i <= mSegments; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(mSegments);

        // Interpolação linear entre inicio e fim
        Vector2 currentPos = mStart + totalDelta * t;

        float offsetAmount = sinf(t * Math::Pi * frequency) * mAmplitude * (1.0f - mAnimProgress);

        // Calcula vetor perpendicular para aplicar o offset
        Vector2 dir = totalDelta;
        dir.Normalize();
        Vector2 perpendicular(-dir.y, dir.x); // Rotaciona 90 graus

        currentPos += perpendicular * offsetAmount;

        // Calcula o segmento individual (delta entre ponto anterior e atual)
        Vector2 segDelta = currentPos - lastPos;
        float segLength = segDelta.Length();

        if (segLength > 0.0f) {
            // Centro do segmento para posicionamento
            Vector2 center = lastPos + segDelta * 0.5f;

            float rotation = atan2f(segDelta.y, segDelta.x);

            Vector2 size(segLength * 1.2f, mSegmentHeight);

            renderer->DrawTexture(
                center,
                size,
                rotation,
                Vector3(1.0f, 1.0f, 1.0f),
                mTexture,
                Vector4::UnitRect,
                GetGame()->GetCamera()->GetPosCamera(),
                Vector2::One,
                1.0f,
                1.0f
            );
        }

        lastPos = currentPos;
    }
}