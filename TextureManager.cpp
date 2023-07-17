#include <stdio.h>

#include "TextureManager.h"


// Static members
std::unordered_map<std::string, Texture*> TextureManager::loadedTextures;
SDL_Renderer* TextureManager::renderer;

void TextureManager::Init(SDL_Renderer* _renderer)
{
	renderer = _renderer;
}

Texture* TextureManager::Load(const std::string& filepath)
{
	// If the texture actually exists then we can return it
	if (loadedTextures[filepath] != nullptr)
	{
		return loadedTextures[filepath];
	}

	//loadedTextures.emplace(filepath, Texture(renderer, filepath));
	loadedTextures[filepath] = new Texture(renderer, filepath);

	if (loadedTextures[filepath]->Get() == nullptr)
	{
		printf("Texture manager failed to load texture from: %s! SDL_image Error: %s\n", filepath.c_str(), IMG_GetError());
		// remove the texture as it is already nullptr
		loadedTextures.erase(filepath);
		return nullptr;
	}
	return loadedTextures[filepath];
}

void TextureManager::UnLoad(const std::string& filepath)
{
	delete loadedTextures[filepath];
	
	loadedTextures.erase(filepath);
	// it returns a size_t so we may log if none was deleted
}

void TextureManager::UnLoadAll()
{
	for (auto it = loadedTextures.cbegin(); it != loadedTextures.cend();)
	{
		UnLoad(it++->first);
	}
}

Texture::Texture()
{
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

void Texture::SetRenderer(SDL_Renderer* _renderer)
{
	renderer = _renderer;
}

bool Texture::LoadTextureFromFile(const std::string& filepath)
{
	_texture = IMG_LoadTexture(renderer, filepath.c_str());
	if (_texture == nullptr)
	{
		printf("Texture failed to load! From path: %s\n", filepath.c_str());
		return false;
	}
	return true;
}

void Texture::Destroy()
{
	SDL_DestroyTexture(_texture);
}
