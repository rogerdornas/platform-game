//
// Created by roger on 23/11/2025.
//

#pragma once

#include "DrawComponent.h"
#include <string>

class DrawRopeComponent : public DrawComponent {
public:
    DrawRopeComponent(class Actor* owner, const std::string& texturePath, int drawOrder = 102);
    ~DrawRopeComponent() override;

    void Draw(class Renderer* renderer) override;

    void SetEndpoints(Vector2 start, Vector2 end);

    void SetAnimationProgress(float progress) { mAnimProgress = progress; }

    // Configurações visuais
    void SetNumSegments(int segments) { mSegments = segments; }
    void SetAmplitude(float amplitude) { mAmplitude = amplitude; }
    void SetSegmentHeight(float segmentHeight) { mSegmentHeight = segmentHeight; }

private:
    class Texture* mTexture;
    Vector2 mStart;
    Vector2 mEnd;

    float mAnimProgress;
    int mSegments;
    float mAmplitude;
    float mSegmentHeight;
};