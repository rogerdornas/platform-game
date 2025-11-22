#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"

enum class RendererMode
{
    TRIANGLES,
    LINES
};

class Renderer
{
public:
	Renderer(SDL_Window* window);
	~Renderer();

	bool Initialize(float width, float height);
	void InitializeFadeQuad();
	void Shutdown();
	void UnloadAllTextures();

    void DrawRect(const Vector2 &position, const Vector2 &size,  float rotation,
                  const Vector3 &color, const Vector2 &cameraPos, RendererMode mode, float alpha = 1.0f);

    void DrawTexture(const Vector2 &position, const Vector2 &size,  float rotation,
                     const Vector3 &color, Texture *texture,
                     const Vector4 &textureRect = Vector4::UnitRect,
                     const Vector2 &cameraPos = Vector2::Zero, Vector2 scale = Vector2::One,
                     float textureFactor = 1.0f, float alpha = 1.0f, float freezeLevel = 0.0f);

    void DrawGeometry(const Vector2 &position, const Vector2 &size,  float rotation,
                      const Vector3 &color, const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode);

	void DrawLine(const Vector2 &start, const Vector2 &end, const Vector3 &color,
						float thickness, const Vector2 &cameraPos, float alpha);

	void DrawFade(float alpha);

    void Clear();
    void Present();

	void BeginGameDraw();
	void BeginUIDraw();

	// Nova função para ser chamada quando a janela for redimensionada
	void OnWindowResize(float newWidth, float newHeight);

	void SetZoom(float zoom);
	// Getter para o zoom atual
	float GetZoom() const { return mZoom; }
	// Getters para a resolução virtual "zoomada" (útil para a Câmera)
	float GetZoomedWidth() const { return mZoomedWidth; }
	float GetZoomedHeight() const { return mZoomedHeight; }

    // Getters
    class Texture* GetTexture(const std::string& fileName);
	class Font* GetFont(const std::string& fileName);
	class Shader* GetBaseShader() const { return mBaseShader; }
	class VertexArray* GetSpriteVerts() const { return mSpriteVerts; }
	// Getters para a resolução virtual (útil para o Game)
	float GetVirtualWidth() const { return mVirtualWidth; }
	float GetVirtualHeight() const { return mVirtualHeight; }

	Vector2 ScreenToVirtual(const Vector2& screenPoint) const;

	// Nova função helper para calcular e definir o viewport
	void UpdateViewport();

	// === Iluminação ===
	void SetAmbientLight(const Vector3& color, float intensity);
	void AddLight(class Light* light);
	void ClearLights();

private:
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color, float alpha = 1.0f, Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect,
              float textureFactor = 1.0f);

	bool LoadShaders();
    void CreateSpriteVerts();
	void UploadLightingUniforms(); // envia luzes ao shader
	void DeactivateLighting();

	void SetShader(Shader* shader);

	// Game
	class Game* mGame;

	// Basic shader
	class Shader* mBaseShader;
	class Shader* mFadeShader;

    // Sprite vertex array
    class VertexArray *mSpriteVerts;
	VertexArray* mFadeVAO;

	// Window
	SDL_Window* mWindow;

	// OpenGL context
	SDL_GLContext mContext;

	// Ortho projection for 2D shaders
	Matrix4 mOrthoProjection;

    // Map of textures loaded
    std::unordered_map<std::string, class Texture*> mTextures;

	// === Dados de iluminação ===
	Vector3 mAmbientColor;
	float mAmbientIntensity;
	std::vector<Light*> mLights;

	// === Novas variáveis para controle de resolução ===
	float mVirtualWidth;  // Largura da nossa cena (ex: 1920)
	float mVirtualHeight; // Altura da nossa cena (ex: 1080)
	float mWindowWidth;   // Largura atual da janela
	float mWindowHeight;  // Altura atual da janela

	float mZoom;          // Nível de zoom atual
	float mZoomedWidth;   // mVirtualWidth / mZoom
	float mZoomedHeight;  // mVirtualHeight / mZoom

	int mViewportX;
	int mViewportY;
	int mViewportWidth;
	int mViewportHeight;

	bool mDrawingUI;
};