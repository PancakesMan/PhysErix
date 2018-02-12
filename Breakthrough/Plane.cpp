#include <Renderer2D.h>
#include <glm\glm.hpp>

#include "Plane.h"
#include "RigidBody.h"

Plane::Plane() : Plane(glm::vec2(0,1), 0)
{
}

Plane::Plane(glm::vec2 normal, float distance) : PhysicsObject(PLANE), m_normal(normal), m_distanceToOrigin(distance)
{
	m_colour = glm::vec4(1, 1, 1, 1);
	m_normal = glm::normalize(m_normal);
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
	float lineSegmentLength = 300000;
	glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
	glm::vec2 parallel(m_normal.y, - m_normal.x);

	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);

	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawLine(start.x, start.y, end.x, end.y);
}

void Plane::resetPosition()
{
}

void Plane::resolveCollision(RigidBody* actor)
{
	float elasticity = 1;
	float j = glm::dot(-(1 + elasticity) * actor->getVelocity(), m_normal) / (1 / actor->getMass());

	glm::vec2 force = m_normal * j;
	actor->applyForce(force);
}