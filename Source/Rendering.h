#pragma once

#include "TextureManager.h"
#include "Physics.h"
#include "ECS.h"


struct Sprite
{
	Sprite() = default;
	Sprite(int width, int height, SDL_Rect* _src, Texture* _texture) 
	{
		dst = { 0,0, width, height };

		if (_src)
			src = *_src;
		else
			src = { 0, 0, _texture->GetWidth(), _texture->GetHeight() };

		texture = _texture;
	}
	SDL_Rect dst;
	SDL_Rect src;
	Texture* texture;
};

class Camera
{

};

inline COMPONENT(Sprite);

namespace RenderSystem
{

void Render(Vector2 camera);



}