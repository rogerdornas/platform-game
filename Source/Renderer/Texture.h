#pragma once
#include <string>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	bool LoadFromSurface(SDL_Surface* surface);
	void Unload();

	void SetActive(int index = 0) const;

    static GLenum SDLFormatToGL(SDL_PixelFormat* fmt);

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	unsigned int GetTextureID() const { return mTextureID; }

	void SetFromGLTexture(GLuint texID, int width, int height);

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};
