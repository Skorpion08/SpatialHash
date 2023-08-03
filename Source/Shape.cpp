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
	angle += angleRadians;
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

void Polygon::AddPosition(const Vector2& addPosition)
{
	if (addPosition.x == 0.0f && addPosition.y == 0.0f)
		return;

	center.x += addPosition.x;
	center.y += addPosition.y;
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].x += addPosition.x;
		vertices[i].y += addPosition.y;
	}
}

void Polygon::AddRotation(float addRotation)
{
	if (addRotation == 0.0f)
		return;

	// Apply rotation to every vertex
	for (int i = 0; i < vertices.size(); i++)
	{
		Vector2& vertex = vertices[i];

		// Rotation matrix			
		float xPrim = center.x + (vertex.x - center.x) * cos(addRotation) - (vertex.y - center.y) * sin(addRotation);
		float yPrim = center.y + (vertex.x - center.x) * sin(addRotation) + (vertex.y - center.y) * cos(addRotation);

		vertex.x = xPrim;
		vertex.y = yPrim;
	}
}

OBB::OBB(float width, float height, Vector2 position, float rotation)
{
	// Top left corner
	float x = position.x - width / 2.0f;
	float y = position.y - height / 2.0f;

	w = width;
	h = height;

	vertices.reserve(4);
	sides.resize(2);

	center = position;

	vertices.emplace_back(x,   y);
	vertices.emplace_back(x,   y+h);
	vertices.emplace_back(x+w, y+h);
	vertices.emplace_back(x+w, y);

	AddRotation(rotation);
}

void OBB::UpdateSideVectors()
{
	// Loop through two sides (it's all we need for a rectangle)
	for (uint8_t i = 0; i < 2 ; ++i)
	{
		sides[i] = Vector2(vertices[i], vertices[i + 1]);
	}
}

AABB::AABB(float width, float height, Vector2 position) : OBB(width, height, position)
{
	/*
	// Calculates the top left corner, assumes the position is centered
	x = position.x - width / 2.0f;
	y = position.y - height / 2.0f;

	w = width;
	h = height;

	vertices.reserve(4);
	sides.resize(2);

	vertices.emplace_back(x,   y);
	vertices.emplace_back(x,   y+h);
	vertices.emplace_back(x+w, y+h);
	vertices.emplace_back(x+w, y);
	*/
}

void AABB::UpdateSideVectors()
{
	// Loop through two sides (it's all we need for a rectangle)
	for (uint8_t i = 0; i < 2; ++i)
	{
		sides[i] = Vector2(vertices[i], vertices[i + 1]);
	}
}
