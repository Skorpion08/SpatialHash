#include <vector>

#include "Shape.h"

void Shape2D::ApplyRotation(float angleAddRad)
{
	SetRotation(angleRad + angleAddRad);
}

void Shape2D::SetRotation(float angleRadians)
{
	// Apply rotation to every vertex
	for (int i = 0; i < vertices.size(); i++)
	{
		Point2& vertex = vertices[i];

		// Rotation matrix			
		float xPrim = center.x + (vertex.x - center.x) * cos(angleRadians) - (vertex.y - center.y) * sin(angleRadians);
		float yPrim = center.y + (vertex.x - center.x) * sin(angleRadians) + (vertex.y - center.y) * cos(angleRadians);

		vertex.x = xPrim;
		vertex.y = yPrim;
	}
}
