#include <vector>

#include "Shape.h"

void Shape2D::ApplyRotation(float angleRadians)
{
	// Apply rotation to every vertex
	for (int i = 0; i < vertices.size(); i++)
	{
		Vector2& vertex = vertices[i];

		// Rotation matrix			
		float xPrim = center.x + (vertex.x - center.x) * cos(angleRadians) - (vertex.y - center.y) * sin(angleRadians);
		float yPrim = center.y + (vertex.x - center.x) * sin(angleRadians) + (vertex.y - center.y) * cos(angleRadians);

		vertex.x = xPrim;
		vertex.y = yPrim;
	}
	angleRad += angleRadians;
}

std::vector<Vector2>& Shape2D::GetSideVectors_DEPRECATED()
{
	std::vector<Vector2> sides;
	sides.reserve(vertices.size());

	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		// vertices[i] is the current vertex
		// vertices[(i + 1) % vertices.size()] is the next vertex (if it's the last one modulo will change it into 0)
		sides.emplace_back(Vector2(vertices[i], vertices[(i + 1) % vertices.size()]));
	}
	return sides;
}

void Shape2D::UpdateSideVectors()
{
	size_t vertexCount = vertices.size();

	// Loop through all vertices except the last one because it's a special case
	for (size_t i = 0; i < vertexCount - 1; ++i)
	{
		sides[i] = Vector2(vertices[i], vertices[i+1]);
	}

	// The last side
	sides[vertexCount - 1] = Vector2(vertices[vertexCount - 1], vertices[0]);
}
