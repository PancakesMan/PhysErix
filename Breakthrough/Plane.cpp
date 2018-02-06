#include <Renderer2D.h>

#include "Plane.h"

Plane::Plane() : Plane(glm::vec2(0,1), 0)
{
}

Plane::Plane(glm::vec2 normal, float distance) : PhysicsObject(PLANE), m_normal(normal), m_distanceToOrigin(distance)
{
	m_colour = glm::vec4(1, 1, 1, 1);
}

Plane::~Plane()
{
}

void Plane::fixedUpdate(glm::vec2 gravity, float timeStep)
{
}

void Plane::debug()
{
}

void Plane::draw(aie::Renderer2D * renderer)
{
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
	glm::vec2 parallel(m_normal.y - m_normal.x);

	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);

	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawLine(start.x, start.y, end.x, end.y, 2.0f);
}

void Plane::resetPosition()
{
}
