#include "UIFont.h"
#include <vector>
#include <SDL_image.h>

#include "../Renderer/Renderer.h"
#include "../Renderer/Texture.h"

UIFont::UIFont(Renderer* renderer)
    :mRenderer(renderer)
{
}

UIFont::~UIFont()
{
}

bool UIFont::Load(const std::string& fileName)
{
	// We support these font sizes
	std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
								  34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

	for (int size: fontSizes) {
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (!font) {
			SDL_Log("Erro ao carregar fonte tamanho %d: %s", size, TTF_GetError());
			return false;
		}
		mFontData[size] = font;
	}
	return true;
}

void UIFont::Unload()
{
	for (auto& pair : mFontData) {
		if (pair.second) {
			TTF_CloseFont(pair.second);
		}
	}
	mFontData.clear();
}

Texture* UIFont::RenderText(const std::string& text, const Vector3& color /*= Color::White*/,
                                int pointSize /*= 24*/, unsigned wrapLength /*= 900*/)
{
	if (!mRenderer)
	{
		SDL_Log("Renderer é nulo. Não é possível renderizar texto!");
		return nullptr;
	}

	// Converte Vector3 para SDL_Color
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// Encontra a fonte mais próxima do tamanho solicitado
	TTF_Font* font = nullptr;
	auto it = mFontData.find(pointSize);
	if (it == mFontData.end())
	{
		int closestSize = -1;
		int smallestDiff = std::numeric_limits<int>::max();
		for (const auto& pair : mFontData)
		{
			int diff = std::abs(pair.first - pointSize);
			if (diff < smallestDiff)
			{
				smallestDiff = diff;
				closestSize = pair.first;
			}
		}
		if (closestSize != -1)
			font = mFontData[closestSize];
		else
		{
			SDL_Log("Nenhuma fonte carregada!");
			return nullptr;
		}
	}
	else
	{
		font = it->second;
	}

	// Renderiza o texto para uma surface SDL
	SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), sdlColor, wrapLength);
	if (!surface)
	{
		SDL_Log("Falha ao renderizar texto: %s", TTF_GetError());
		return nullptr;
	}

	// Cria uma textura OpenGL a partir da surface
	auto* tex = new Texture();
	if (tex->LoadFromSurface(surface))
	{
		SDL_FreeSurface(surface);
		return tex;
	}
	else {
		delete tex;
		SDL_FreeSurface(surface);
		return nullptr;
	}
}
