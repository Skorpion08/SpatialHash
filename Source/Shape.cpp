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

void Shape2D::GetSideVectors(std::vector<Vector2>& outSides)
{
	outSides.resize(vertices.size());

	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		// vertices[i] is the current vertex
		// vertices[(i + 1) % vertices.size()] is the next vertex (if it's the last one modulo will change it into 0)
		outSides.emplace_back(Vector2(vertices[i], vertices[(i + 1) % vertices.size()]));
	}
}
