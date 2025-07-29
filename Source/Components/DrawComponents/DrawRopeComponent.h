//
// Created by roger on 28/07/2025.
//

#pragma once

#include "DrawComponent.h"
#include <string>

class DrawRopeComponent : public DrawComponent {
public:
    // (Lower draw order corresponds with further back)
    DrawRopeComponent(Actor* owner, const std::string& texturePath, int drawOrder = 102);

    ~DrawRopeComponent() override;

    void Draw(SDL_Renderer* renderer) override;

    void SetEndpoints(Vector2 start, Vector2 end);
    void SetAnimationProgress(float progress) { mAnimProgress = progress; }
    void SetNumSegments(int segments) { mSegments = segments; }
    void SetAmplitude(float amplitude) { mAmplitude = amplitude; }
    void SetSegmentHeight(int segmentHeight) { mSegmentHeight = segmentHeight; }

private:
    SDL_Texture* mTexture;
    Vector2 mStart;
    Vector2 mEnd;
    float mAnimProgress = 0.0f;
    int mSegments = 20;
    float mAmplitude = 12.0f;
    float mSegmentHeight = 8;
};
