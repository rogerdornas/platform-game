//
// Created by Lucas N. Ferreira on 28/05/25.
//

#pragma once

#include <string>
#include <SDL_image.h>
#include "UIElement.h"

class UIImage : public UIElement
{
public:
    UIImage(const std::string& imagePath, const Vector2 &pos = Vector2::Zero,
            const Vector2 &size = Vector2(100.f, 100.f), const Vector3 &color = Color::White, SDL_Renderer* renderer = nullptr);

    ~UIImage();

    void SetImage(const std::string& imagePath);
    void SetTransparency(int transparency) { mTransparency = transparency; }
    void Draw(SDL_Renderer* renderer, const Vector2 &screenPos) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    SDL_Texture* mTexture; // Texture for the image
    SDL_Renderer* mRenderer;
    int mTransparency;
};
