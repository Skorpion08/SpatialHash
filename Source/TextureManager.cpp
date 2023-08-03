#include <stdio.h>

#include "TextureManager.h"

TextureManager::TextureManager()
{
}

void TextureManager::Init(SDL_Renderer* _renderer)
{
	renderer = _renderer;
}

Texture* TextureManager::Load(const std::string& filepath)
{
	// If the texture actually exists then we can return it
	if (textures[filepath] != nullptr)
	{
		return textures[filepath];
	}

	//textures.emplace(filepath, Texture(renderer, filepath));
	textures[filepath] = new Texture(renderer, filepath);

	if (textures[filepath]->Get() == nullptr)
	{
		printf("\tFrom: %s! SDL_image Error: %s\n", filepath.c_str(), IMG_GetError());
		// remove the texture as it is already nullptr
		textures.erase(filepath);
		return nullptr;
	}
	return textures[filepath];
}

void TextureManager::UnLoad(const std::string& filepath)
{
	delete textures[filepath];
	
	textures.erase(filepath);
	// it returns a size_t so we may log if none was deleted
}

void TextureManager::UnLoadAll()
{
	for (auto it = textures.cbegin(); it != textures.cend();)
	{
		UnLoad(it++->first);
	}
}

Texture::Texture()
{
	_texture = nullptr;
	renderer = nullptr;

	width = 0;
	height = 0;
}

Texture::Texture(SDL_Renderer* _renderer, const std::string& filepath)
{
	renderer = _renderer;
	LoadTextureFromFile(filepath);
}

Texture::~Texture()
{
	Destroy();
}

void Texture::Render(const SDL_Rect* srcRect, const SDL_Rect* dstRect, float angle, const SDL_Point* center, SDL_RendererFlip flip)
{
	//SDL_RenderCopy(renderer, _texture, srcRect, dstRect);
	SDL_RenderCopyEx(renderer, _texture, srcRect, dstRect, angle, center, flip);
}

void Texture::SetRenderer(SDL_Renderer* _renderer)
{
	renderer = _renderer;
}

bool Texture::LoadTextureFromFile(const std::string& filepath)
{
	_texture = IMG_LoadTexture(renderer, filepath.c_str());

	if (_texture == nullptr)
	{
		printf("Texture failed to load!\n");
		return false;
	}
	
	SDL_QueryTexture(_texture, nullptr, nullptr, &width, &height);

	return true;
}

void Texture::Destroy()
{
	SDL_DestroyTexture(_texture);
}
