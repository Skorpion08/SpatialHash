#include "Physics.h"

void Physics::UpdateTransform(double deltaTime)
{
	std::vector<Archetype*> query = ECS::Query({ getID(Transform), getID(Kinematics) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Transform* transform = static_cast<Column<Transform>*>(arch->columns[arch->type.FindIndexFor(getID(Transform))])->Get(0);
		Kinematics* kinematics = static_cast<Column<Kinematics>*>(arch->columns[arch->type.FindIndexFor(getID(Kinematics))])->Get(0);
		for (int i = 0; i < arch->entityCount; ++i)
		{
			kinematics[i].vel += kinematics[i].acc * deltaTime;

			transform[i].pos += kinematics[i].vel * deltaTime;
		}
	}
}