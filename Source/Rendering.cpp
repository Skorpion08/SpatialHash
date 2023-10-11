#include "Rendering.h"

void RenderSystem::Render(Vector2 camera)
{
	std::vector<Archetype*> query = ECS::Query({ getID(Sprite) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Sprite* sprite = static_cast<Column<Sprite>*>(arch->columns[arch->type.FindIndexFor(getID(Sprite))])->Get(0);

		if (arch->type.FindIndexFor(getID(Transform)) == -1)
		{
			for (int i = 0; i < arch->entityCount; ++i)
			{
				sprite[i].texture->Render(&sprite[i].src, &sprite[i].dst);
			}
		}
		else
		{
			Transform* transform = static_cast<Column<Transform>*>(arch->columns[arch->type.FindIndexFor(getID(Transform))])->Get(0);

			for (int i = 0; i < arch->entityCount; ++i)
			{
				sprite[i].dst.x = (transform[i].pos.x - sprite[i].dst.w / 2.f) - camera.x;
				sprite[i].dst.y = (transform[i].pos.y - sprite[i].dst.h / 2.f) - camera.y;

				sprite[i].texture->Render(&sprite[i].src, &sprite[i].dst, transform[i].rotation * PI);
			}
		}

	}
}
