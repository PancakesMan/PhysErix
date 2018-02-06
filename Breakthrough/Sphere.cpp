#include <glm\glm.hpp>
#include <Renderer2D.h>

#include "Sphere.h"


Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) :
	RigidBody(SPHERE, position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
}

Sphere::~Sphere()
{
}

void Sphere::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawCircle(m_position.x, m_position.y, m_radius);
}

bool Sphere::checkCollision(PhysicsObject* pOther)
{
	Sphere* other = dynamic_cast<Sphere*>(pOther);
	return other != nullptr && glm::distance(other->getPosition(), m_position) < m_radius + other->getRadius();
}
