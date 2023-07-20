#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <unordered_map>

class Texture
{
public:
	Texture();
	Texture(SDL_Renderer* _renderer, const std::string& filepath);
	virtual ~Texture();

	void Render(const SDL_Rect* srcRect, const SDL_Rect* dstRect);

	void SetRenderer(SDL_Renderer* _renderer);

	bool LoadTextureFromFile(const std::string& filepath);

	void Destroy();

	// some methods

	SDL_Texture* Get() const { return  _texture; }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
	SDL_Texture* _texture;

	SDL_Renderer* renderer;

	int width, height;
};


class TextureManager
{
	TextureManager();
public:
	// Prevent copying
	TextureManager(TextureManager const&) = delete;
	TextureManager& operator=(TextureManager const&) = delete;

	static void Init(SDL_Renderer* renderer);

	static Texture* Load(const std::string& filepath);

	static void UnLoad(const std::string& filepath);

	static void UnLoadAll();

private:
	// It's a way to store currently loaded textures
	// Key: filepath
	// Value: texture
	static std::unordered_map<std::string, Texture*> textures;

	static SDL_Renderer* renderer;

};
