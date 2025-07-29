//
// Created by roger on 28/07/2025.
//

#include "DrawRopeComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawRopeComponent::DrawRopeComponent(Actor *owner, const std::string &texturePath, int drawOrder)
    :DrawComponent(owner, drawOrder)
{
    mTexture = GetGame()->LoadTexture(texturePath);
}

DrawRopeComponent::~DrawRopeComponent() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void DrawRopeComponent::SetEndpoints(Vector2 start, Vector2 end) {
    mStart = start;
    mEnd = end;
}

void DrawRopeComponent::Draw(SDL_Renderer* renderer) {
    if (!mIsVisible) {
        return;
    }

    if (mAnimProgress >= 1.0f) {
        return;
    }

    Vector2 last = mStart;

    for (int i = 1; i <= mSegments; ++i) {
        float t = static_cast<float>(i) / mSegments;
        Vector2 current = mStart + (mEnd - mStart) * t;

        float frequency = 4.0f; // número de ondas ao longo da corda
        float offset = sinf(t * Math::Pi * frequency) * mAmplitude * (1.0f - mAnimProgress);
        Vector2 normal = Vector2(mEnd.y - mStart.y, -(mEnd.x - mStart.x));
        normal.Normalize();
        current += normal * offset;

        Vector2 delta = current - last;
        float angle = atan2f(delta.y, delta.x) * (180.0f / Math::Pi);

        SDL_Rect dest;
        dest.x = static_cast<int>(last.x) - delta.Length() / 2 - GetGame()->GetCamera()->GetPosCamera().x;
        dest.y = static_cast<int>(last.y) - static_cast<int>(mSegmentHeight) / 2 - GetGame()->GetCamera()->GetPosCamera().y;
        dest.w = static_cast<int>(delta.Length());
        dest.h = static_cast<int>(mSegmentHeight);

        SDL_RenderCopyEx(renderer, mTexture, nullptr, &dest, angle, nullptr, SDL_FLIP_NONE);

        last = current;
    }
}
